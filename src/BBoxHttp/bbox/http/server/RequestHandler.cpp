/**
 * @file
 *
 * Implementation for the bbox::http::server::RequestHandler class.
 */

#include <bbox/http/server/RequestHandler.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/Assert.h>

namespace bbox
{
    namespace http
    {
        namespace server
        {

            RequestHandler::RequestHandler(const std::string &name, rt::Service &parent, HttpServer &server)
                : bbox::rt::Resource(name, parent)
                , m_server(server)
                , m_is_logged_in(false)
                , m_prefix()
                , m_handler_func()
                , m_debug_enable("debug", *this)
            {
            }

            RequestHandler::~RequestHandler()
            {
            }

            void RequestHandler::Login(std::string &&prefix, HandlerFunc &&handler)
            {
                BBOX_ASSERT(!m_is_logged_in);
                BBOX_ASSERT(m_server.GetOverallRunLevel() == bbox::rt::RunLevel::RUNNING);

                m_is_logged_in = true;
                m_prefix = std::move(prefix);
                m_handler_func = std::move(handler);

                auto insert_result = m_server.m_request_handlers.insert(this);

                BBOX_ASSERT(insert_result.second);
            }

            void RequestHandler::Logout()
            {
                BBOX_ASSERT(m_is_logged_in);

                auto erase_count = m_server.m_request_handlers.erase(this);

                BBOX_ASSERT(erase_count == 1);

                m_is_logged_in = false;
                m_handler_func.clear();
                m_prefix.clear();
            }

            void RequestHandler::HandleStopping()
            {
                BBOX_ASSERT(!m_is_logged_in);

                RequestStopAllChildren();
                NotifyStopped();
            }

            void RequestHandler::PrintState(bbox::DebugOutput &out) const
            {
                if (m_is_logged_in)
                {
                    out << "Server: ";
                    m_server.PrintResourcePathLink(out);
                    out << std::endl;

                    out.Format("Prefix: %s\n", m_prefix);
                }
                else
                {
                    out.Format("Not logged in\n");
                }
            }

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox
