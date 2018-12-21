/**
 * @file
 *
 * Implementation for the bbox::http::server::ServerWebSocket class.
 */

#include <bbox/http/server/ServerWebSocket.h>
#include <bbox/http/server/details/Connection.h>
#include <bbox/http/server/details/WebSocketConnection.h>

#include <bbox/Assert.h>
#include <boost/beast/websocket.hpp>

namespace bbox {
namespace http {
namespace server {

ServerWebSocket ServerWebSocket::Upgrade(Request &request, const std::string &protocol, StateHandler &&state_handler, ReceiveHandler &&rx_handler)
{
    BBOX_ASSERT(details::WebSocketConnection::IsUpgrade(request));

    ServerWebSocket result;
    result.m_connection = new details::WebSocketConnection(&result, request, protocol, std::move(state_handler), std::move(rx_handler));

    return result;
}

ServerWebSocket::~ServerWebSocket()
{
    Close();
}

bool ServerWebSocket::IsOpen() const
{
    bool result = false;

    if (m_connection)
    {
        result = m_connection->IsOpen();
    }

    return result;
}


void ServerWebSocket::Close()
{
    if (m_connection)
    {
        m_connection->Close();
        m_connection->UpdateOwner(nullptr);
        m_connection = nullptr;
    }
}

void ServerWebSocket::Send(const std::string &str)
{
    if (m_connection)
    {
        m_connection->Send(str);
    }
}

} // namespace bbox::http::server
} // namespace bbox::http
} // namespace bbox
