/**
* @file
*
* Implementation for the bbox::http::Request class.
*/

#include <bbox/http/Response.h>
#include <bbox/http/server/HttpServer.h>

#include <bbox/Assert.h>
#include <bbox/FromString.h>

#include <pion/http/response.hpp>
#include <pion/http/response_writer.hpp>

namespace bbox {
    namespace http {

        struct Response::Pimpl
        {
            server::HttpServer *m_server_service;
            pion::http::server_ptr m_server;
            Request::AutoFailureHandlerPtr m_request_auto_failure;
            pion::http::request_ptr m_request;
            pion::tcp::connection_ptr m_connection;
            pion::http::response_writer_ptr m_writer;
            bool m_finish_completed;

            Pimpl(Request &request)
                : m_server_service(request.m_server_service)
                , m_server(request.m_server)
                , m_request_auto_failure(request.m_auto_failure)
                , m_request(request.m_request)
                , m_connection(request.m_connection)
                , m_writer()
                , m_finish_completed(false)
            {
            }

            ~Pimpl()
            {
                BBOX_ASSERT(m_finish_completed);
            }

            void CreateWriter(const std::shared_ptr<Pimpl> &shared_ptr)
            {
                m_writer = pion::http::response_writer::create(
                    m_connection,
                    *m_request,
                    boost::bind(&Pimpl::Finish, shared_ptr));
            }

            void Finish()
            {
                m_connection->finish();

                m_writer.reset();

                m_server_service->NotifyRequestCompleted();

                // Set the variable here to ensure that
                // the destructor is not called
                // when the m_writer.reset() call is made.

                m_finish_completed = true;
            }
        };

        Response::Response(Request &request)
            : m_pimpl(std::make_shared<Pimpl>(request))
        {
            m_pimpl->CreateWriter(m_pimpl);
        }

        Response::~Response()
        {
        }

        Response &Response::SetLifecycleForceClose()
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_connection->set_lifecycle(pion::tcp::connection::LIFECYCLE_CLOSE);

            return *this;
        }

        Response &Response::SetResponse(int code, const std::string &msg)
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().set_status_code(code);
            m_pimpl->m_writer->get_response().set_status_message(msg);

            return *this;
        }

        Response &Response::SetContent(const std::string &content)
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().set_content_length(content.size());
            m_pimpl->m_writer->write(content);

            return *this;
        }

        Response &Response::SetContent_NoCopy(const void *data, size_t length)
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().set_content_length(length);
            m_pimpl->m_writer->write_no_copy(data, length);

            return *this;
        }

        Response &Response::SetHeader(const std::string &header, const std::string &value)
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().add_header(header, value);

            return *this;
        }

        Response &Response::SetHeader_NoCache()
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().add_header("Cache-Control", "private, max-age=0, no-cache, no-store");

            return *this;
        }

        Response &Response::SetHeader_ContentType(const std::string &type)
        {
            BBOX_ASSERT(m_pimpl);

            m_pimpl->m_writer->get_response().set_content_type(type);

            return *this;
        }
        
        void Response::Send()
        {
            BBOX_ASSERT(m_pimpl);
            BBOX_ASSERT(m_pimpl->m_request_auto_failure->NotHandled());

            m_pimpl->m_request_auto_failure->SetHandled();

            m_pimpl->m_writer->send();

            m_pimpl.reset();
        }

    } // namespace bbox::http
} // namespace bbox
