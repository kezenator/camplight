/**
  * @file
  *
  * Implementation for the bbox::http::server::Connection class.
  */

#include <bbox/http/server/Connection.h>
#include <bbox/http/server/HttpServer.h>

#include <bbox/Assert.h>
#include <bbox/Error.h>
#include <bbox/Format.h>

#include <boost/beast/http/read.hpp>

namespace bbox
{
    namespace http
    {
        namespace server
        {

			Connection::Connection(HttpServer &server, boost::asio::ip::tcp::socket &&socket, HttpServer::HandlerFunc server_handler)
				: m_server(server)
				, m_socket(std::move(socket))
				, m_server_handler(std::move(server_handler))
				, m_state_string("Reading request #1")
				, m_buffer()
				, m_request_ptr()
				, m_response_ptr()
				, m_request_count(1)
				, m_closed(false)
			{
				m_request_ptr = std::make_shared<Request::RequestType>();

				boost::beast::http::async_read(
					m_socket,
					m_buffer,
					*m_request_ptr,
					std::bind(&Connection::OnRead, this, std::placeholders::_1, std::placeholders::_2));
			}

			Connection::~Connection()
			{
			}

			void Connection::Send(Response::ResponsePtr &&response)
			{
				BBOX_ASSERT(!m_request_ptr);
				BBOX_ASSERT(!m_response_ptr);
				BBOX_ASSERT(!m_closed);

				m_state_string = bbox::Format("Sending request #%d", m_request_count);

				m_response_ptr = std::move(response);
				m_response_ptr->prepare_payload();

				boost::beast::http::async_write(
					m_socket,
					*m_response_ptr,
					std::bind(&Connection::OnWrite, this, std::placeholders::_1, std::placeholders::_2, m_response_ptr->need_eof()));
			}

			void Connection::OnRead(boost::system::error_code ec, std::size_t bytes_transferred)
			{
				BBOX_ASSERT(m_request_ptr);
				BBOX_ASSERT(!m_response_ptr);
				BBOX_ASSERT(!m_closed);

				if (ec)
				{
					m_request_ptr.reset();
					m_state_string = bbox::Format("Error reading request #%d: %s", m_request_count, bbox::Error(ec).ToString());
					m_closed = true;
					m_server.ConnectionClosed(this);
					return;
				}
				else
				{
					m_state_string = bbox::Format("Handling request #%d", m_request_count);

					Request request(m_server, this, std::move(m_request_ptr));
					m_request_ptr.reset();

					m_server.HandleRequest(std::move(request), m_server_handler);
				}
			}

			void Connection::OnWrite(boost::system::error_code ec, std::size_t bytes_transferred, bool close)
			{
				BBOX_ASSERT(!m_request_ptr);
				BBOX_ASSERT(m_response_ptr);
				BBOX_ASSERT(!m_closed);

				m_response_ptr.reset();

				if (ec)
				{
					m_state_string = bbox::Format("Error sending request #%d: %s", m_request_count, bbox::Error(ec).ToString());
					m_closed = true;
					m_server.ConnectionClosed(this);
					return;
				}
				else if (close)
				{
					m_state_string = bbox::Format("Closing gracefully after %d requests", m_request_count);
					m_closed = true;
					m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
					m_server.ConnectionClosed(this);
					return;
				}
				else
				{
					m_request_count += 1;
					m_state_string = bbox::Format("Reading request #%d", m_request_count);

					m_request_ptr = std::make_shared<Request::RequestType>();

					boost::beast::http::async_read(
						m_socket,
						m_buffer,
						*m_request_ptr,
						std::bind(&Connection::OnRead, this, std::placeholders::_1, std::placeholders::_2));
				}
			}

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox
