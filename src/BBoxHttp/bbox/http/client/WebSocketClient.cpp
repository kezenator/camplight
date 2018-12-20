/**
 * @file
 *
 * Implementation for the bbox::http::client::WebSocketClient class.
 */

#include <bbox/http/client/WebSocketClient.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>
#include <bbox/rt/Timer.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace bbox {
namespace http {
namespace client {

struct WebSocketClient::UriDecode
{
    UriDecode()
        : valid(false)
        , uri()
        , protocol()
        , host()
        , port()
        , resource()
    {
    }

    ~UriDecode() = default;

    UriDecode(const UriDecode &other) = default;
    UriDecode(UriDecode &&other) = default;
    UriDecode &operator =(const UriDecode &other) = default;
    UriDecode &operator =(UriDecode &&other) = default;

    explicit UriDecode(const std::string &_uri, const std::string &_protocol)
        : valid(false)
        , uri(_uri)
        , protocol(_protocol)
        , host()
        , port(0)
        , resource()
    {
        if ((uri.size() > 5)
            && (uri.substr(0, 5) == "ws://"))
        {
            auto resource_pos = uri.find('/', 5);

            if (resource_pos == std::string::npos)
            {
                host = uri.substr(5);
                resource = "/";
            }
            else
            {
                host = uri.substr(5, resource_pos - 5);
                resource = uri.substr(resource_pos);
            }

            valid = true;

            port = "80";

            auto colon_pos = host.rfind(':');
            if (colon_pos != std::string::npos)
            {
                port = host.substr(colon_pos + 1);
                host = host.substr(0, colon_pos);
            }
        }
    }

    std::string ToString() const
    {
        if (valid)
        {
            return Format("%s / %s => %s / %s / %s", uri, protocol, host, port, resource);
        }
        else
        {
            return Format("%s / %s => <INVALID>", uri, protocol);
        }
    }

    bool valid;
    std::string uri;
    std::string protocol;
    std::string host;
    std::string port;
    std::string resource;
};

struct WebSocketClient::Pimpl
{
    Pimpl(WebSocketClient &parent, StateHandler state_handler, ReceiveHandler receive_handler)
        : m_parent(parent)
        , m_retry_timer("retry-timer", m_parent)
        , m_state_handler(std::move(state_handler))
        , m_receive_handler(std::move(receive_handler))
        , m_stage_str("constructed")
        , m_opened(false)
        , m_connected(false)
        , m_reading(false)
        , m_sending(false)
        , m_pending_sends()
        , m_requested_uri()
        , m_cur_uri()
		, m_resolver(m_parent.GetProactor().GetIoService())
		, m_socket(m_parent.GetProactor().GetIoService())
	{
		m_retry_timer.SetHandler(std::bind(&Pimpl::HandleRetryTimerExpired, this));
	}

	WebSocketClient &m_parent;
	rt::Timer m_retry_timer;
	StateHandler m_state_handler;
	ReceiveHandler m_receive_handler;
    std::string m_stage_str;
	bool m_opened;
	bool m_connected;
	bool m_reading;
	bool m_sending;
	std::list<std::string> m_pending_sends;
    UriDecode m_requested_uri;
    UriDecode m_cur_uri;

	boost::asio::ip::tcp::resolver m_resolver;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_socket;
	boost::beast::multi_buffer m_rx_buffer;

	void Open(const std::string &uri, const std::string &protocol)
	{
		BBOX_ASSERT(m_parent.GetLocalRunLevel() == rt::RunLevel::RUNNING);

        if (m_opened)
            CloseConnection();
        else
            m_retry_timer.StartSingleShot(rt::TimeSpan::Milliseconds(10));

		m_opened = true;
        m_requested_uri = UriDecode(uri, protocol);
	}

	void CloseConnection()
	{
		if (m_connected)
			m_socket.lowest_layer().cancel();
	}

	void Send(const std::string &str)
	{
		if (m_connected)
		{
			m_pending_sends.push_back(str);

			if (!m_sending)
			{
				m_sending = true;
				m_socket.async_write(
					boost::asio::buffer(m_pending_sends.front()),
					std::bind(
						&Pimpl::HandleWritten,
						this,
						std::placeholders::_1,
						std::placeholders::_2));
			}
		}
	}

	void PrintState(DebugOutput &out) const
	{
        out.Format("Stage Str:     %s\n", m_stage_str);
        out.Format("Opened:        %s\n", m_opened);
        out.Format("Requested URI: %s\n", m_requested_uri.ToString());
        out.Format("Current URI:   %s\n", m_cur_uri.ToString());
        out.Format("Connected:     %s\n", m_connected);
	}

	void HandleStopping()
	{
		CloseConnection();
		CheckStopping();
	}

	void CheckStopping()
	{
		bool stopped = false;

		if (m_parent.GetLocalRunLevel() == rt::RunLevel::STOPPING)
		{
			if (m_retry_timer.IsRunning())
			{
				// Waiting for retry - we can stop immeidately
				stopped = true;
			}
		}

		if (stopped)
		{
			m_retry_timer.Cancel();

			m_parent.RequestStopAllChildren();
			m_parent.NotifyStopped();
		}
	}

	void HandleRetryTimerExpired()
	{
		BBOX_ASSERT(m_parent.GetLocalRunLevel() == rt::RunLevel::RUNNING);
		BBOX_ASSERT(m_opened);

        m_cur_uri = m_requested_uri;
        m_stage_str = "resolving";

		m_resolver.async_resolve(
			m_cur_uri.host,
			m_cur_uri.port,
			std::bind(
				&Pimpl::HandleResolved,
				this,
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void HandleResolved(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
	{
		if (!ec
			&& (m_parent.GetLocalRunLevel() != rt::RunLevel::RUNNING))
		{
			ec = make_error_code(boost::system::errc::connection_aborted);
		}

		if (ec)
		{
			HandleError("resolve", ec);
		}
		else
		{
            m_stage_str = "connecting";

			boost::asio::async_connect(
				m_socket.next_layer(),
				results.begin(),
				results.end(),
				std::bind(
					&Pimpl::HandleConnect,
					this,
					std::placeholders::_1));
		}
	}

	void HandleConnect(boost::system::error_code ec)
	{
		if (!ec
			&& (m_parent.GetLocalRunLevel() != rt::RunLevel::RUNNING))
		{
			ec = make_error_code(boost::system::errc::connection_aborted);
		}

		if (ec)
		{
			HandleError("connect", ec);
		}
		else
		{
            m_stage_str = "handshaking";

			m_socket.async_handshake_ex(
				m_cur_uri.host,
				m_cur_uri.resource,
				[this](auto &m)
				{
					m.insert(boost::beast::http::field::sec_websocket_protocol, m_cur_uri.protocol);
				},
				std::bind(
					&Pimpl::HandleHandshake,
					this,
					std::placeholders::_1));
		}
	}

	void HandleHandshake(boost::system::error_code ec)
	{
		if (!ec
			&& (m_parent.GetLocalRunLevel() != rt::RunLevel::RUNNING))
		{
			ec = make_error_code(boost::system::errc::connection_aborted);
		}

		if (ec)
		{
			HandleError("handshake", ec);
		}
		else
		{
            m_stage_str = "connected";
			m_connected = true;
			StartRead();

			m_state_handler(bbox::Error());
		}
	}

	void StartRead()
	{
		BBOX_ASSERT(m_connected);
		BBOX_ASSERT(!m_reading);

		m_reading = true;

		m_socket.async_read(
			m_rx_buffer,
			std::bind(
				&Pimpl::HandleRead,
				this,
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void HandleRead(
		boost::system::error_code ec,
		std::size_t /*bytes_transferred*/)
	{
		BBOX_ASSERT(m_reading);
		m_reading = false;

		if (ec)
		{
			HandleError("read", ec);
		}
		else
		{
			bool got_text = m_socket.got_text();
			std::string text;

			if (got_text)
			{
				text.resize(m_rx_buffer.size());

				size_t offset = 0;
				for (const auto &buf : m_rx_buffer.data())
				{
					memcpy(&text[offset], buf.data(), buf.size());
					offset += buf.size();
				}
			}

			m_rx_buffer.consume(m_rx_buffer.size());

			StartRead();

			if (got_text)
			{
				m_receive_handler(text);
			}
			else
			{
				// Don't handle binary messages
				CloseConnection();
			}
		}
	}

	void HandleWritten(
		boost::system::error_code ec,
		std::size_t /*bytes_transferred*/)
	{
		BBOX_ASSERT(m_sending);
		m_sending = false;
		m_pending_sends.pop_front();

		if (ec)
		{
			HandleError("send", ec);
		}
		else if (!m_pending_sends.empty())
		{
			m_sending = true;
			m_socket.async_write(
				boost::asio::buffer(m_pending_sends.front()),
				std::bind(
					&Pimpl::HandleWritten,
					this,
					std::placeholders::_1,
					std::placeholders::_2));
		}
	}

	void HandleError(std::string operation, boost::system::error_code ec)
	{
		DebugOutput out(BBOX_FUNC, DebugOutput::Testing);
		out.Format("WebSocketClient: Op=%s, Stage=%s, Error=%s\n",
            operation,
            m_stage_str,
            bbox::Error(ec).ToString());

		if (m_connected)
		{
			m_connected = false;
			m_state_handler(bbox::Error(ec));
		}

		if (!m_reading
			&& !m_sending)
		{
			m_pending_sends.clear();
			m_retry_timer.StartSingleShot(rt::TimeSpan::Seconds(10));
		}

		CheckStopping();
	}
};

WebSocketClient::WebSocketClient(
	const std::string &name,
	rt::Resource &parent,
	StateHandler state_handler,
	ReceiveHandler receive_handler)
	: rt::Resource(name, parent)
	, m_pimpl(std::make_unique<Pimpl>(*this, std::move(state_handler), std::move(receive_handler)))
{
}

WebSocketClient::WebSocketClient(
	const std::string &name,
	rt::Service &parent,
	StateHandler state_handler,
	ReceiveHandler receive_handler)
	: rt::Resource(name, parent)
	, m_pimpl(std::make_unique<Pimpl>(*this, std::move(state_handler), std::move(receive_handler)))
{
}

WebSocketClient::~WebSocketClient()
{
}

bool WebSocketClient::IsOpen() const
{
	return m_pimpl->m_connected;
}

void WebSocketClient::Open(const std::string &uri, const std::string &protocol)
{
	m_pimpl->Open(uri, protocol);
}

void WebSocketClient::CloseConnection()
{
	m_pimpl->CloseConnection();
}

void WebSocketClient::Send(const std::string &str)
{
	m_pimpl->Send(str);
}

void WebSocketClient::PrintState(DebugOutput &out) const
{
	m_pimpl->PrintState(out);
}

void WebSocketClient::HandleStopping()
{
	m_pimpl->HandleStopping();
}

} // namespace bbox::http::client
} // namespace bbox::http
} // namespace bbox
