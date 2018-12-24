/**
 * @file
 *
 * Implementation for the bbox::http::server::details::WebSocketConnection class.
 */

#include <bbox/http/server/details/WebSocketConnection.h>
#include <bbox/http/server/details/Connection.h>
#include <bbox/http/server/ServerWebSocket.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/Assert.h>

#include <boost/beast/version.hpp>

namespace bbox {
namespace http {
namespace server {
namespace details {

WebSocketConnection::WebSocketConnection(ServerWebSocket *owner, Request &request, const std::string &protocol, StateHandler &&state_handler, ReceiveHandler &&rx_handler)
    : m_owner(owner)
    , m_server(request.m_pimpl_ptr->m_server)
    , m_stream(std::move(request.m_pimpl_ptr->m_connection_ptr->m_socket))
    , m_state_handler(std::move(state_handler))
    , m_rx_handler(std::move(rx_handler))
    , m_state_str("Accepting")
    , m_outstanding_async_ops(0)
    , m_accept_completed(false)
    , m_close_requested(false)
    , m_close_reported(false)
    , m_write_pending(false)
{
    BBOX_ASSERT(m_owner != nullptr);

    m_server.WebSocketCreated(this);

    request.m_pimpl_ptr->m_connection_ptr->NotifyUpgradedToWebSocket();
    request.m_pimpl_ptr->SetHandled();

    m_outstanding_async_ops += 1;
    m_stream.async_accept_ex(
        *request.m_pimpl_ptr->m_request_ptr,
        [=](auto & m)
        {
            m.insert(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            m.insert(boost::beast::http::field::sec_websocket_protocol, protocol);
        },
        std::bind(&WebSocketConnection::OnAccept, this, std::placeholders::_1));
}

WebSocketConnection::~WebSocketConnection()
{
}

bool WebSocketConnection::IsUpgrade(Request &request)
{
    if (!request)
        return false;
    if (!boost::beast::websocket::is_upgrade(*request.m_pimpl_ptr->m_request_ptr))
        return false;
    return true;
}

void WebSocketConnection::UpdateOwner(ServerWebSocket *new_owner)
{
    BBOX_ASSERT(m_owner != nullptr);

    m_owner = new_owner;

    if (m_owner == nullptr)
    {
        Close();
    }
}

bool WebSocketConnection::IsOpen() const
{
    return m_accept_completed
        && !m_close_requested;
}

void WebSocketConnection::Close()
{
    if (!m_close_requested)
    {
        m_close_requested = true;
        m_stream.next_layer().cancel();

        CheckClose();
    }
}

void WebSocketConnection::Send(const std::string &str)
{
    if (!m_close_requested)
    {
        m_pending_writes.emplace_back(str);
        StartWrite();
    }
}

void WebSocketConnection::CheckClose()
{
    if (m_close_requested
        && !m_close_reported
        && (m_outstanding_async_ops == 0)
        && !m_write_pending)
    {                
        if (m_owner != nullptr)
        {
            m_owner->m_connection = nullptr;
            m_owner = nullptr;
        }

        m_close_reported = true;
        m_server.WebSocketClosed(this);
    }
}

void WebSocketConnection::ReportError(const bbox::Error &error)
{
    BBOX_ASSERT(error);

    if (!m_close_requested)
    {
        m_close_requested = true;

        m_state_handler(error);

        m_stream.next_layer().close();
    }
}

void WebSocketConnection::StartWrite()
{
    if (!m_close_requested
        && !m_write_pending
        && !m_pending_writes.empty())
    {
        std::string str{ std::move(m_pending_writes.front()) };
        m_pending_writes.pop_front();

        boost::asio::buffer_copy(
            m_tx_buffer.prepare(str.size()),
            boost::asio::buffer(str));
        m_tx_buffer.commit(str.size());

        m_write_pending = true;
        m_outstanding_async_ops += 1;

        m_stream.text(true);
        m_stream.async_write(
            m_tx_buffer.data(),
            std::bind(
                &WebSocketConnection::OnWrite,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }
}

void WebSocketConnection::OnAccept(boost::system::error_code ec)
{
    BBOX_ASSERT(m_outstanding_async_ops > 0);
    m_outstanding_async_ops -= 1;

    if (ec)
    {
        ReportError(bbox::Error(ec));
    }
    else
    {
        // We're now connected - start reading,
        // and also notify the application

        if (!m_close_requested)
        {
            m_accept_completed = true;

            m_outstanding_async_ops += 1;
            m_stream.async_read(
                m_rx_buffer,
                std::bind(
                    &WebSocketConnection::OnRead,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

            m_state_handler(bbox::Error());
        }
    }

    CheckClose();
}

void WebSocketConnection::OnRead(boost::system::error_code ec, size_t bytes_transferred)
{
    BBOX_ASSERT(m_outstanding_async_ops > 0);
    m_outstanding_async_ops -= 1;

    if (ec)
    {
        ReportError(bbox::Error(ec));
    }
    else
    {
        // Start a new read and
        // pass it onto the application

        if (!m_close_requested)
        {
            bool got_text = m_stream.got_text();
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

            m_outstanding_async_ops += 1;
            m_stream.async_read(
                m_rx_buffer,
                std::bind(
                    &WebSocketConnection::OnRead,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

            if (got_text)
            {
                m_rx_handler(text);
            }
            else
            {
                // Don't handle binary messages
                Close();
            }
        }
    }

    CheckClose();
}

void WebSocketConnection::OnWrite(boost::system::error_code ec, size_t bytes_transferred)
{
    BBOX_ASSERT(m_outstanding_async_ops > 0);
    BBOX_ASSERT(m_write_pending == true);

    m_outstanding_async_ops -= 1;
    m_write_pending = false;

    if (ec)
    {
        ReportError(bbox::Error(ec));
    }
    else
    {
        // Write any more pending data

        if (!m_close_requested)
        {
            m_tx_buffer.consume(m_tx_buffer.size());

            StartWrite();
        }
    }

    CheckClose();
}

} // namespace bbox::http::sever::details
} // namespace bbox::http::server
} // namespace bbox::http
} // namespace bbox
