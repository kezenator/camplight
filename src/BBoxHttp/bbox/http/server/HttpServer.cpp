/**
* @file
*
* Implementation for the bbox::http::server::HttpServer class.
*/

#include <bbox/http/server/HttpServer.h>
#include <bbox/http/server/RequestHandler.h>
#include <bbox/Assert.h>
#include <bbox/DebugOutput.h>

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

            HttpServer::HttpServer(const std::string &name, rt::Service &parent)
                : rt::Service(name, parent)
                , m_check_shutdown_work("check-shutdown-work", *this, boost::bind(&HttpServer::CheckShutdown, this))
                , m_scheduler("scheduler", *this)
				, m_num_requests_received(0)
                , m_num_outstanding_requests(0)
                , m_request_handlers()
                , m_default_debug_enable("default-handler-debug", *this)
            {
            }

            HttpServer::~HttpServer()
            {
                BBOX_ASSERT(m_servers.empty());
                BBOX_ASSERT(m_num_outstanding_requests == 0);
                BBOX_ASSERT(m_request_handlers.empty());
            }

            void HttpServer::HandleStarting()
            {
                BBOX_ASSERT(m_num_outstanding_requests == 0);
                BBOX_ASSERT(m_request_handlers.empty());

                NotifyStarted();
            }

            void HttpServer::HandleStopping()
            {
                BBOX_ASSERT(m_request_handlers.empty());

                for (pion::http::server_ptr &server : m_servers)
                    server->async_stop(false, boost::bind(&HttpServer::ServerStopped, this, server));

                m_check_shutdown_work.Schedule();
            }

            void HttpServer::CheckShutdown()
            {
                if ((GetLocalRunLevel() == rt::RunLevel::STOPPING)
                    && m_servers.empty()
                    && (m_num_outstanding_requests == 0))
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

			void HttpServer::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Num requests received: %d\n", m_num_requests_received);
				out.Format("Num outstanding requests: %d\n", m_num_outstanding_requests);

				out.Format("Num endpoints: %d\n", m_servers.size());
				for (const auto &server_ptr : m_servers)
				{
                    out.Format("    Endpoint: ");
                    {
                        bbox::DebugOutput url_out(BBOX_FUNC, out, DebugOutput::Mime_Text_Url);
                        url_out.Format(
                            "http://%s:%d/",
                            server_ptr->get_endpoint().address().to_string(),
                            server_ptr->get_endpoint().port());
                    }
                    out << std::endl;
				}

                out.Format("Num request handlers: %s\n", m_request_handlers.size());
                for (const auto &handler_ptr : m_request_handlers)
                {
                    out.Format("    Handler: %s @ ", handler_ptr->m_prefix);
                    handler_ptr->PrintResourcePathLink(out);
                    out << std::endl;
                }
			}

            void HttpServer::ServerStopped(pion::http::server_ptr &server)
            {
                for (auto it = m_servers.begin();
                     it != m_servers.end();
                     ++it)
                {
                    if (*it == server)
                    {
                        m_servers.erase(it);
                        m_check_shutdown_work.Schedule();
                        return;
                    }
                }
            }

            void HttpServer::NotifyRequestCompleted()
            {
                BBOX_ASSERT((GetOverallRunLevel() == rt::RunLevel::RUNNING)
                    || (GetOverallRunLevel() == rt::RunLevel::STOPPING));

                BBOX_ASSERT(m_num_outstanding_requests > 0);

                m_num_outstanding_requests -= 1;

                if ((m_num_outstanding_requests == 0)
                    && (GetLocalRunLevel() == rt::RunLevel::STOPPING))
                {
                    m_check_shutdown_work.Schedule();
                }
            }

            bool HttpServer::AddServer(const rt::net::TcpEndpoint &tcp_endpoint,
                                       HandlerFunc handler /* = HandlerFunc() */)
            {
                BBOX_ASSERT(GetOverallRunLevel() == rt::RunLevel::RUNNING);

                boost::system::error_code ec;

                pion::http::server_ptr server(new pion::http::server(m_scheduler, tcp_endpoint));

                server->add_resource("/", boost::bind(&HttpServer::HandleRequest, this, server, _1, _2, std::move(handler)));
                server->start();

                m_servers.push_back(server);

                DebugOutput out(BBOX_FUNC, DebugOutput::Activity);
                out << "Opened HTTP server " << server->get_endpoint() << std::endl;

                return server->get_endpoint() == tcp_endpoint;
            }

            unsigned short HttpServer::AddUnassignedPortServer(const rt::net::IpAddress &ip_address,
                                                               HandlerFunc handler /* = HandlerFunc() */)
            {
                BBOX_ASSERT(GetOverallRunLevel() == rt::RunLevel::RUNNING);

                boost::system::error_code ec;

                boost::asio::ip::tcp::endpoint endpoint(ip_address, 0);

                pion::http::server_ptr server(new pion::http::server(m_scheduler, endpoint));

                server->add_resource("/", boost::bind(&HttpServer::HandleRequest, this, server, _1, _2, std::move(handler)));
                server->start();

                m_servers.push_back(server);

                DebugOutput out(BBOX_FUNC, DebugOutput::Activity);
                out << "Opened HTTP server " << server->get_endpoint() << std::endl;

                return server->get_endpoint().port();
            }

            void HttpServer::TryAndOpenWebBrowserToServer(const std::string &path)
            {
#ifdef WIN32
                if (!m_servers.empty())
                {
                    // Determine the address of the first server

                    pion::http::server_ptr server_ptr = m_servers.front();

                    bbox::rt::net::TcpEndpoint endpoint(server_ptr->get_endpoint());

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

            void HttpServer::HandleRequest(const pion::http::server_ptr &server,
                                           const pion::http::request_ptr &http_request_ptr,
                                           const pion::tcp::connection_ptr &tcp_conn,
                                           const HandlerFunc &user_handler)
            {
                // Mark that one more request is pending

				m_num_requests_received += 1;
				m_num_outstanding_requests += 1;

                // Construct a request object

                Request request(*this, server, http_request_ptr, tcp_conn);

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
                            out << "HTTP Request (via default handler): "
                                << tcp_conn->get_remote_endpoint()
                                << " => "
                                << server->get_endpoint()
                                << " "
                                << http_request_ptr->get_method()
                                << " "
                                << http_request_ptr->get_resource()
                                << (http_request_ptr->get_query_string().empty() ? "" : "?")
                                << http_request_ptr->get_query_string()
                                << std::endl;
                        }

                        handler_ptr->m_handler_func(request);
                        return;
                    }
                }

                // Pass the request on to the default handler.
                // If it's not set the auto-cleardown of the
                // request will send a failure response

                if (user_handler)
                {
                    DebugOutput out(BBOX_FUNC, m_default_debug_enable);
                    if (out)
                    {
                        out << "HTTP Request (via default handler): "
                            << tcp_conn->get_remote_endpoint()
                            << " => "
                            << server->get_endpoint()
                            << " "
                            << http_request_ptr->get_method()
                            << " "
                            << http_request_ptr->get_resource()
                            << (http_request_ptr->get_query_string().empty() ? "" : "?")
                            << http_request_ptr->get_query_string()
                            << std::endl;
                    }

                    user_handler(request);
                    return;
                }

                DebugOutput out(BBOX_FUNC, DebugOutput::Error);
                if (out)
                {
                    out << "HTTP Request (Unhandled): "
                        << tcp_conn->get_remote_endpoint()
                        << " => "
                        << server->get_endpoint()
                        << " "
                        << http_request_ptr->get_method()
                        << " "
                        << http_request_ptr->get_resource()
                        << (http_request_ptr->get_query_string().empty() ? "" : "?")
                        << http_request_ptr->get_query_string()
                        << std::endl;
                }
            }

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox
