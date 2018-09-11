/**
 * @file
 *
 * Header for the bbox::http::server::details::Connection class.
 */

#ifndef __BBOX__HTTP__SERVER__DETAILS__CONNECTION_H__
#define __BBOX__HTTP__SERVER__DETAILS__CONNECTION_H__

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <bbox/rt/net/TcpEndpoint.h>

#include <bbox/http/Request.h>
#include <bbox/http/Response.h>
#include <bbox/http/server/HttpServer.h>

namespace bbox {
namespace http {
namespace server {
namespace details {

//Forward declarations
class WebSocketConnection;

/**
 * A connection that is reading requests and serving content.
 */
class Connection
{
	friend class HttpServer;
	friend class WebSocketConnection;

	Connection() = delete;
	Connection(const Connection &) = delete;
	Connection &operator =(const Connection &) = delete;

public:
	~Connection();

	bbox::rt::net::TcpEndpoint GetLocalEndpoint() const { return m_socket.local_endpoint(); }
	bbox::rt::net::TcpEndpoint GetRemoteEndpoint() const { return m_socket.remote_endpoint(); }
	const std::string &GetState() const { return m_state_string; }

	void Send(Response::ResponsePtr &&response);
	void NotifyUpgradedToWebSocket();

private:
	Connection(HttpServer &server, boost::asio::ip::tcp::socket &&socket, HttpServer::HandlerFunc server_handler);

	void OnRead(boost::system::error_code ec, std::size_t bytes_transferred);
	void OnWrite(boost::system::error_code ec, std::size_t bytes_transferred, bool close);

	HttpServer &m_server;
	boost::asio::ip::tcp::socket m_socket;
	HttpServer::HandlerFunc m_server_handler;
	std::string m_state_string;
	boost::beast::flat_buffer m_buffer;
	Request::RequestPtr m_request_ptr;
	Response::ResponsePtr m_response_ptr;
	std::uint64_t m_request_count;
	bool m_closed;
};

} // namespace bbox::http::server::details
} // namespace bbox::http::server
} // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__SERVER__DETAILS__CONNECTION_H__
