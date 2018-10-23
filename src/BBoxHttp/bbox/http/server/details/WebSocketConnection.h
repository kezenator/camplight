/**
 * @file
 *
 * Header for the bbox::http::server::details::WebSocketConnection class.
 */

#ifndef __BBOX__HTTP__SERVER__DETAILS__WEB_SOCKET_CONNECTION_H__
#define __BBOX__HTTP__SERVER__DETAILS__WEB_SOCKET_CONNECTION_H__

#include <deque>

#include <bbox/http/Request.h>
#include <bbox/Error.h>

#include <boost/beast/websocket.hpp>

namespace bbox {
namespace http {
namespace server {
namespace details {

/**
 * Manages a connection to a web-socket.
 */
class WebSocketConnection
{
public:
	using StateHandler = std::function<void(const bbox::Error &error)>;
	using ReceiveHandler = std::function<void(const std::string &message)>;

	WebSocketConnection(Request &request, const std::string &protocol, StateHandler &&state_handler, ReceiveHandler &&rx_handler);
	~WebSocketConnection();

	static bool IsUpgrade(Request &request);

	bbox::rt::net::TcpEndpoint GetLocalEndpoint() const { return m_stream.next_layer().local_endpoint(); }
	bbox::rt::net::TcpEndpoint GetRemoteEndpoint() const { return m_stream.next_layer().remote_endpoint(); }
	const std::string &GetState() const { return m_state_str; }

	bool IsOpen() const;
	void Close();
	void Send(const std::string &str);

private:

	void CheckClose();
	void ReportError(const bbox::Error &error);
	void StartWrite();

	void OnAccept(boost::system::error_code ec);
	void OnRead(boost::system::error_code ec, size_t bytes_transferred);
	void OnWrite(boost::system::error_code ec, size_t bytes_transferred);

	HttpServer &m_server;
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_stream;
	StateHandler m_state_handler;
	ReceiveHandler m_rx_handler;

	std::string m_state_str;
	size_t m_outstanding_async_ops;

	bool m_accept_completed;
	bool m_close_requested;
	bool m_close_reported;
	bool m_write_pending;

	boost::beast::multi_buffer m_rx_buffer;

	std::deque<std::string> m_pending_writes;
	boost::beast::multi_buffer m_tx_buffer;
};

} // namespace bbox::http::sever::details
} // namespace bbox::http::server
} // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__SERVER__DETAILS__WEB_SOCKET_CONNECTION_H__
