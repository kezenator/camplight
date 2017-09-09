/**
* @file
*
* Implementation for the bbox::http::server::HttpServer class.
*/

#include <bbox/http/server/HttpServer.h>
#include <bbox/Assert.h>
#include <bbox/DebugOutput.h>

namespace bbox {
    namespace http {
        namespace server {

            HttpServer::HttpServer(const std::string &name, rt::Service &parent)
                : rt::Service(name, parent)
                , m_check_shutdown_work("check-shutdown-work", *this, boost::bind(&HttpServer::CheckShutdown, this))
                , m_scheduler("scheduler", *this)
				, m_num_requests_received(0)
                , m_num_outstanding_requests(0)
            {
            }

            HttpServer::~HttpServer()
            {
                BBOX_ASSERT(m_servers.empty());
                BBOX_ASSERT(m_num_outstanding_requests == 0);
            }

            void HttpServer::HandleStarting()
            {
                BBOX_ASSERT(m_num_outstanding_requests == 0);

                NotifyStarted();
            }

            void HttpServer::HandleStopping()
            {
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
                    out.Format("Endpoint: ");
                    {
                        bbox::DebugOutput url_out(BBOX_FUNC, out, DebugOutput::Mime_Text_Url);
                        url_out.Format(
                            "http://%s:%d/",
                            server_ptr->get_endpoint().address().to_string(),
                            server_ptr->get_endpoint().port());
                    }
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
                                       RequestHandler handler)
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
                                                               RequestHandler handler)
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

            void HttpServer::HandleRequest(const pion::http::server_ptr &server,
                                           const pion::http::request_ptr &http_request_ptr,
                                           const pion::tcp::connection_ptr &tcp_conn,
                                           const RequestHandler &user_handler)
            {
                DebugOutput out(BBOX_FUNC, DebugOutput::Activity);
                out << "HTTP Request: " 
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

                // Mark that one more request is pending

				m_num_requests_received += 1;
				m_num_outstanding_requests += 1;

                // Construct a request object

                Request request(*this, server, http_request_ptr, tcp_conn);

                // Pass the response on to the user handler.
                // If it's not set the auto-cleardown of the
                // request will send a failure response

                if (user_handler)
                {
                    user_handler(request);
                }
            }

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox
