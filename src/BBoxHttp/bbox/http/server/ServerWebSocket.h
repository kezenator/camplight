/**
 * @file
 *
 * Header for the bbox::http::server::ServerWebSocket class.
 */

#ifndef __BBOX__HTTP__SERVER__SERVER_WEB_SOCKET_H__
#define __BBOX__HTTP__SERVER__SERVER_WEB_SOCKET_H__

#include <bbox/http/Request.h>
#include <bbox/Error.h>
#include <bbox/http/server/details/WebSocketConnection.h>

#include <functional>

namespace bbox {
namespace http {
namespace server {

// Forward declarations
namespace details {
class WebSocketConnection;
}

/**
 * A handle to upgrade and access a web-socket connection.
 */
class ServerWebSocket
{
    friend details::WebSocketConnection;

public:

    using StateHandler = std::function<void (const bbox::Error &error)>;
    using ReceiveHandler = std::function<void (const std::string &message)>;

    ServerWebSocket()
        : m_connection(nullptr)
    {
    }

    ~ServerWebSocket();

    ServerWebSocket(const ServerWebSocket &) = delete;
    ServerWebSocket &operator =(const ServerWebSocket &) = delete;

    ServerWebSocket(ServerWebSocket &&other)
        : m_connection(other.m_connection)
    {
        other.m_connection = nullptr;

        if (m_connection)
            m_connection->UpdateOwner(this);
    }

    ServerWebSocket &operator =(ServerWebSocket &&other)
    {
        std::swap(m_connection, other.m_connection);

        if (m_connection)
            m_connection->UpdateOwner(this);

        if (other.m_connection)
            other.m_connection->UpdateOwner(&other);

        return *this;
    }

    static ServerWebSocket Upgrade(Request &request, const std::string &protocol, StateHandler &&state_handler, ReceiveHandler &&rx_handler);

    bool IsOpen() const;
    void Close();

    void Send(const std::string &str);

private:

    details::WebSocketConnection *m_connection;
};

} // namespace bbox::http::server
} // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__SERVER__SERVER_WEB_SOCKET_H__
