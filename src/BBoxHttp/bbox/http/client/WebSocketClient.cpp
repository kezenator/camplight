/**
 * @file
 *
 * Implementation for the bbox::http::client::WebSocketClient class.
 */

#include <bbox/http/client/WebSocketClient.h>
#include <bbox/Assert.h>
#include <bbox/rt/Timer.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace bbox {
namespace http {
namespace client {

struct WebSocketClient::Pimpl
{
	Pimpl(WebSocketClient &parent, StateHandler state_handler, ReceiveHandler receive_handler)
		: m_parent(parent)
		, m_retry_timer("retry-timer", m_parent)
		, m_state_handler(std::move(state_handler))
		, m_receive_handler(std::move(receive_handler))
		, m_opened(false)
		, m_connected(false)
		, m_reading(false)
		, m_sending(false)
		, m_pending_sends()
		, m_resolver(m_parent.GetProactor().GetIoService())
		, m_socket(m_parent.GetProactor().GetIoService())
	{
		m_retry_timer.SetHandler(std::bind(&Pimpl::HandleRetryTimerExpired, this));
	}

	WebSocketClient &m_parent;
	rt::Timer m_retry_timer;
	StateHandler m_state_handler;
	ReceiveHandler m_receive_handler;
	bool m_opened;
	bool m_connected;
	bool m_reading;
	bool m_sending;
	std::list<std::string> m_pending_sends;
	std::string m_host;
	std::string m_port;
	std::string m_resource;
	std::string m_protocol;

	boost::asio::ip::tcp::resolver m_resolver;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_socket;
	boost::beast::multi_buffer m_rx_buffer;

	void Open(const std::string &host, const std::string &resource, const std::string &protocol)
	{
		BBOX_ASSERT(m_parent.GetLocalRunLevel() == rt::RunLevel::RUNNING);
		BBOX_ASSERT(!m_opened); // Currently don't support re-opening

		m_opened = true;
		m_host = host;
		m_port = "8080";
		m_resource = resource;
		m_protocol = protocol;
		m_retry_timer.StartSingleShot(rt::TimeSpan::Milliseconds(10));
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
		out.Format("Connected: %s\n", m_connected);
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

		m_resolver.async_resolve(
			m_host,
			m_port,
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
			HandleError(ec);
		}
		else
		{
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
			HandleError(ec);
		}
		else
		{
			m_socket.async_handshake_ex(
				m_host,
				m_resource,
				[this](auto &m)
				{
					m.insert(boost::beast::http::field::sec_websocket_protocol, m_protocol);
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
			HandleError(ec);
		}
		else
		{
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
			HandleError(ec);
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
			HandleError(ec);
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

	void HandleError(boost::system::error_code ec)
	{
		DebugOutput out(BBOX_FUNC, DebugOutput::Testing);
		out.Format("WebSocketClient: Error %s\n", bbox::Error(ec).ToString());

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

void WebSocketClient::Open(const std::string &host, const std::string &resource, const std::string &protocol)
{
	m_pimpl->Open(host, resource, protocol);
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
