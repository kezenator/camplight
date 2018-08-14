/**
* @file
*
* Implementation for the bbox::http::server::HttpServer class.
*/

#include <bbox/http/server/HttpServer.h>
#include <bbox/http/server/Connection.h>
#include <bbox/http/server/RequestHandler.h>
#include <bbox/Assert.h>
#include <bbox/DebugOutput.h>
#include <bbox/Error.h>

#ifdef WIN32
#include <bbox/TextCoding.h>
#include <shellapi.h>
#endif // WIN32

namespace bbox {
    namespace http {
        namespace server {

            bool HttpServer::RequestHandlerOrder::operator()(RequestHandler *a, RequestHandler *b) const
            {
                // Longer prefixes first, then order by prefix

                if (a->m_prefix.size() > b->m_prefix.size())
                {
                    return true;
                }
                else if (a->m_prefix.size() < b->m_prefix.size())
                {
                    return false;
                }
                else
                {
                    return a->m_prefix < b->m_prefix;
                }
            }

			struct HttpServer::Listener
			{
				HttpServer &m_server;
				boost::asio::ip::tcp::acceptor m_acceptor;
				boost::asio::ip::tcp::socket m_accepted_socket;
				HandlerFunc m_handler;
				bool m_is_accept_pending;
				bool m_stop_requested;
				std::string m_state_string;

				Listener(HttpServer &server,
							const rt::net::TcpEndpoint &tcp_endpoint,
							HandlerFunc handler)
					: m_server(server)
					, m_acceptor(server.GetProactor().GetIoService())
					, m_accepted_socket(server.GetProactor().GetIoService())
					, m_handler(std::move(handler))
					, m_is_accept_pending(false)
					, m_stop_requested(false)
				{
					boost::system::error_code ec;

					auto fail = [&](const char *state)
					{
						m_state_string = bbox::Format("Open failed during \"%s\": %s", state, bbox::Error(ec).ToString());
					};

					// Open the acceptor
					m_acceptor.open(tcp_endpoint.protocol(), ec);
					if (ec)
					{
						fail("open");
						return;
					}

					// Allow address reuse
					m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
					if (ec)
					{
						fail("set_option");
						return;
					}

					// Bind to the server address
					m_acceptor.bind(tcp_endpoint, ec);
					if (ec)
					{
						fail("bind");
						return;
					}

					// Start listening for connections
					m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
					if (ec)
					{
						fail("listen");
						return;
					}

					m_state_string = "Accepting";

					StartAccept();
				}

				void AsyncStop()
				{
					BBOX_ASSERT(!m_stop_requested);
					BBOX_ASSERT(m_is_accept_pending);

					m_stop_requested = true;
					m_acceptor.cancel();
				}

			private:
				void StartAccept()
				{
					BBOX_ASSERT(!m_stop_requested);
					BBOX_ASSERT(!m_is_accept_pending);

					m_is_accept_pending = true;

					m_acceptor.async_accept(
						m_accepted_socket,
						std::bind(
							&Listener::OnAccept,
							this,
							std::placeholders::_1));
				}

				void OnAccept(const boost::system::error_code &ec)
				{
					BBOX_ASSERT(m_is_accept_pending);
					m_is_accept_pending = false;

					if (m_stop_requested)
					{
						m_state_string = "Waiting for destroy";
						m_stop_requested = false;

						m_accepted_socket.close();
						m_acceptor.close();

						m_server.ListenerStopped(this);

						return;
					}

					if (ec)
					{
						m_state_string = bbox::Format("Accept Error: %s", bbox::Error(ec).ToString());
					}
					else
					{
						auto ptr = std::unique_ptr<Connection>(new Connection(m_server, std::move(m_accepted_socket), m_handler));
						auto key = ptr.get();

						m_server.m_connections[key] = std::move(ptr);

						StartAccept();
					}
				}
			};

            HttpServer::HttpServer(const std::string &name, rt::Service &parent)
                : rt::Service(name, parent)
                , m_check_shutdown_work("check-shutdown-work", *this, std::bind(&HttpServer::CheckShutdown, this))
				, m_listeners()
				, m_connections()
                , m_num_requests_received(0)
                , m_request_handlers()
                , m_default_debug_enable("default-handler-debug", *this)
            {
            }

            HttpServer::~HttpServer()
            {
                BBOX_ASSERT(m_listeners.empty());
				BBOX_ASSERT(m_connections.empty());
                BBOX_ASSERT(m_request_handlers.empty());
            }

            void HttpServer::HandleStarting()
            {
				BBOX_ASSERT(m_listeners.empty());
				BBOX_ASSERT(m_connections.empty());
				BBOX_ASSERT(m_request_handlers.empty());

                NotifyStarted();
            }

            void HttpServer::HandleStopping()
            {
                BBOX_ASSERT(m_request_handlers.empty());

				for (auto &entry : m_listeners)
					entry.first->AsyncStop();

                m_check_shutdown_work.Schedule();
            }

            void HttpServer::CheckShutdown()
            {
				if ((GetLocalRunLevel() == rt::RunLevel::STOPPING)
					&& m_listeners.empty()
					&& m_connections.empty())
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

			void HttpServer::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Num requests received: %d\n", m_num_requests_received);

				out.Format("Num listeners: %d\n", m_listeners.size());
				for (const auto &entry: m_listeners)
				{
					const Listener *listener_ptr = entry.first;

                    out.Format("    Listener: ");
                    {
                        bbox::DebugOutput url_out(BBOX_FUNC, out, DebugOutput::Mime_Text_Url);
                        url_out.Format(
                            "http://%s:%d/",
                            listener_ptr->m_acceptor.local_endpoint().address().to_string(),
							listener_ptr->m_acceptor.local_endpoint().port());
                    }
					out.Format(" (%s)\n", listener_ptr->m_state_string);
				}

				out.Format("Num connections: %d\n", m_connections.size());
				for (const auto &entry : m_connections)
				{
					const Connection *connection_ptr = entry.first;

					out.Format("    Connection: %s => %s (%s)\n",
						connection_ptr->GetRemoteEndpoint().ToString(),
						connection_ptr->GetLocalEndpoint().ToString(),
						connection_ptr->GetState());
				}

				out.Format("Num request handlers: %s\n", m_request_handlers.size());
                for (const auto &handler_ptr : m_request_handlers)
                {
                    out.Format("    Handler: %s @ ", handler_ptr->m_prefix);
                    handler_ptr->PrintResourcePathLink(out);
                    out << std::endl;
                }
			}

            void HttpServer::ListenerStopped(Listener *listener)
            {
				GetProactor().Post([=]()
				{
					auto erase_result = m_listeners.erase(listener);

					BBOX_ASSERT(erase_result == 1);

					if (m_listeners.empty()
						&& (GetLocalRunLevel() == rt::RunLevel::STOPPING))
					{
						m_check_shutdown_work.Schedule();
					}
				});
			}

			void HttpServer::ConnectionClosed(Connection *connection)
			{
				GetProactor().Post([=]()
				{
					auto erase_result = m_connections.erase(connection);

					BBOX_ASSERT(erase_result == 1);

					if (m_connections.empty()
						&& (GetLocalRunLevel() == rt::RunLevel::STOPPING))
					{
						m_check_shutdown_work.Schedule();
					}
				});
			}

            bool HttpServer::AddServer(const rt::net::TcpEndpoint &tcp_endpoint,
                                       HandlerFunc handler /* = HandlerFunc() */)
            {
                BBOX_ASSERT(GetOverallRunLevel() == rt::RunLevel::RUNNING);

				auto ptr = std::make_unique<Listener>(*this, tcp_endpoint, handler);

                DebugOutput out(BBOX_FUNC, DebugOutput::Activity);
                out << "Opened HTTP server " << ptr->m_acceptor.local_endpoint() << std::endl;

				Listener *key = ptr.get();

				m_listeners[key] = std::move(ptr);

				return true;
            }

            unsigned short HttpServer::AddUnassignedPortServer(const rt::net::IpAddress &ip_address,
                                                               HandlerFunc handler /* = HandlerFunc() */)
            {
				BBOX_ASSERT(GetOverallRunLevel() == rt::RunLevel::RUNNING);

				auto ptr = std::make_unique<Listener>(*this, rt::net::TcpEndpoint(ip_address, 0), handler);

				DebugOutput out(BBOX_FUNC, DebugOutput::Activity);
				out << "Opened HTTP server " << ptr->m_acceptor.local_endpoint() << std::endl;

				Listener *key = ptr.get();

				m_listeners[key] = std::move(ptr);

				return true;
            }

            void HttpServer::TryAndOpenWebBrowserToServer(const std::string &path)
            {
#ifdef WIN32
                if (!m_listeners.empty())
                {
                    // Determine the address of the first server

					bbox::rt::net::TcpEndpoint endpoint(m_listeners.begin()->first->m_acceptor.local_endpoint());

                    // If it's 0.0.0.0 then convert it to localhost

                    if (endpoint.GetAddress().is_v4()
                        && (endpoint.GetAddress().to_v4().is_unspecified()))
                    {
                        bbox::rt::net::TcpEndpoint::FromString(bbox::Format("127.0.0.1:%d", endpoint.GetPort()), endpoint);
                    }

                    // Try and open a browser window

                    ::ShellExecuteW(
                        nullptr,
                        nullptr,
                        bbox::TextCoding::Win32_UTF8_to_UTF16(bbox::Format("http://%s%s", endpoint.ToString(), path)).c_str(),
                        nullptr,
                        nullptr,
                        0);
                }
#endif // WIN32
            }

            void HttpServer::HandleRequest(Request &&request, const HandlerFunc &server_handler)
            {
                // Mark that one more request has been handled

				m_num_requests_received += 1;

                // See if there is a registered handler
                // with a prefix that matches
                // the request path

                const std::string resource = request.GetResource();
                for (RequestHandler *handler_ptr : m_request_handlers)
                {
                    if ((resource.size() >= handler_ptr->m_prefix.size())
                        && (resource.substr(0, handler_ptr->m_prefix.size()) == handler_ptr->m_prefix))
                    {
                        DebugOutput out(BBOX_FUNC, handler_ptr->m_debug_enable);
                        if (out)
                        {
                            out << "HTTP Request (via registered handler): "
                                << request.GetRemoteEndpoint().ToString()
                                << " => "
                                << request.GetLocalEndpoint().ToString()
								<< " HOST "
								<< request.GetHost()
                                << " "
                                << request.GetMethodString()
                                << " "
								<< request.GetResource()
                                << request.GetFullQueryString()
                                << std::endl;
                        }

                        handler_ptr->m_handler_func(request);
                        return;
                    }
                }

                // Pass the request on to the default handler.
                // If it's not set the auto-cleardown of the
                // request will send a failure response

                if (server_handler)
                {
                    DebugOutput out(BBOX_FUNC, m_default_debug_enable);
                    if (out)
                    {
                        out << "HTTP Request (via server handler): "
							<< request.GetRemoteEndpoint().ToString()
							<< " => "
							<< request.GetLocalEndpoint().ToString()
							<< " HOST "
							<< request.GetHost()
							<< " "
							<< request.GetMethodString()
							<< " "
							<< request.GetResource()
							<< request.GetFullQueryString()
							<< std::endl;
                    }

					server_handler(request);
                    return;
                }

                DebugOutput out(BBOX_FUNC, DebugOutput::Error);
                if (out)
                {
                    out << "HTTP Request (Unhandled): "
						<< request.GetRemoteEndpoint().ToString()
						<< " => "
						<< request.GetLocalEndpoint().ToString()
						<< " HOST "
						<< request.GetHost()
						<< " "
						<< request.GetMethodString()
						<< " "
						<< request.GetResource()
						<< request.GetFullQueryString()
						<< std::endl;
                }
            }

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox
