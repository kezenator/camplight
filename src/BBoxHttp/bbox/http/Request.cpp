/**
* @file
*
* Implementation for the bbox::http::Request class.
*/

#include <bbox/http/Request.h>
#include <bbox/http/Response.h>
#include <bbox/http/ResourceFileSet.h>
#include <bbox/http/server/HttpServer.h>

#include <bbox/Assert.h>
#include <bbox/FromString.h>

#include <pion/http/response.hpp>
#include <pion/http/response_writer.hpp>
#include <pion/http/types.hpp>

#ifdef _DEBUG
#include <bbox/FileUtils.h>
#endif

namespace bbox {
    namespace http {


        Request::AutoFailureHandler::AutoFailureHandler(const pion::http::server_ptr &server,
                                                        const pion::http::request_ptr &request,
                                                        const pion::tcp::connection_ptr &connection)
            : m_server(server)
            , m_request(request)
            , m_connection(connection)
            , m_handled(false)
        {
        }

        Request::AutoFailureHandler::~AutoFailureHandler()
        {
            BBOX_ASSERT(m_handled);
        }

        void Request::AutoFailureHandler::SetHandled()
        {
            m_handled = true;
        }

        Request::Request()
            : m_server_service(nullptr)
            , m_server()
            , m_request()
            , m_connection()
            , m_auto_failure()
        {
        }
        
        Request::Request(const Request &other)
            : m_server_service(other.m_server_service)
            , m_server(other.m_server)
            , m_request(other.m_request)
            , m_connection(other.m_connection)
            , m_auto_failure(other.m_auto_failure)
        {
        }

        Request::Request(Request &&other)
            : m_server_service(std::move(other.m_server_service))
            , m_server(std::move(other.m_server))
            , m_request(std::move(other.m_request))
            , m_connection(std::move(other.m_connection))
            , m_auto_failure(std::move(other.m_auto_failure))
        {
        }

        Request::Request(server::HttpServer &server_service,
                         const pion::http::server_ptr &server,
                         const pion::http::request_ptr &request,
                         const pion::tcp::connection_ptr &connection)
            : m_server_service(&server_service)
            , m_server(server)
            , m_request(request)
            , m_connection(connection)
            , m_auto_failure(new AutoFailureHandler(server, request, connection))
        {
        }

        Request::~Request()
        {
        }

        Request &Request::operator =(const Request &other)
        {
            if (&other != this)
            {
                m_server_service = other.m_server_service;
                m_server = other.m_server;
                m_request = other.m_request;
                m_connection = other.m_connection;
                m_auto_failure = other.m_auto_failure;
            }
            return *this;
        }

        Request &Request::operator =(Request &&other)
        {
            if (&other != this)
            {
                m_server_service = std::move(other.m_server_service);
                m_server = std::move(other.m_server);
                m_request = std::move(other.m_request);
                m_connection = std::move(other.m_connection);
                m_auto_failure = std::move(other.m_auto_failure);
            }
            return *this;
        }

        bbox::rt::net::IpAddress Request::GetRemoteIpAddress() const
        {
            BBOX_ASSERT(*this);

            return m_auto_failure->m_request->get_remote_ip();
        }

        std::string Request::GetRootUrl() const
        {
            std::stringstream stream;
            stream << "http://" << m_server->get_endpoint();

            return stream.str();
        }

        std::string Request::GetResource() const
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_resource();
        }

        bool Request::HasQuery(const std::string &param)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->has_query(param);
        }

        std::string Request::GetQuery(const std::string &param)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_query(param);
        }

        std::string Request::GetFullQueryString()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_query_string();
        }

        bool Request::HasHeader(const std::string &str)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->has_header(str);
        }

        std::string Request::GetHeader(const std::string &header)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_header(header);
        }

        std::string Request::GetMethod()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_method();
        }
        
        bool Request::IsMethod_GetOrHead()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return (m_auto_failure->m_request->get_method() == pion::http::types::REQUEST_METHOD_GET)
                || (m_auto_failure->m_request->get_method() == pion::http::types::REQUEST_METHOD_HEAD);
        }

        bool Request::IsMethod_Get()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_method() == pion::http::types::REQUEST_METHOD_GET;
        }

        bool Request::IsMethod_Head()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_method() == pion::http::types::REQUEST_METHOD_HEAD;
        }

        bool Request::IsMethod_Put()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return m_auto_failure->m_request->get_method() == pion::http::types::REQUEST_METHOD_PUT;
        }

        std::string Request::GetContent()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            return std::string(
                m_auto_failure->m_request->get_content(),
                m_auto_failure->m_request->get_content_length());
        }

        bool Request::RespondWithResource(const ResourceFileSet &resources)
        {
            return RespondWithResource(resources, m_request->get_resource());
        }

        bool Request::RespondWithResource(const ResourceFileSet &resources, const std::string &resource_path)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            const ResourceFile *file_ptr = resources.FindFile(resource_path);

            if (file_ptr)
            {
                if (IsMethod_GetOrHead())
                {
#ifdef _DEBUG
                    std::vector<uint8_t> data;
                    bbox::Error error = bbox::FileUtils::ReadBinaryFile(file_ptr->GetOrigFileName(), data, 100 * 1024 * 1024);

                    if (error)
                    {
                        RespondWithServerError(bbox::Format("Could not read file: %s", error.ToString()));
                    }
                    else
                    {
                        Response response(*this);

                        response.SetHeader_ContentType(file_ptr->GetMimeType());
                        response.SetHeader("Content-Encoding", "identity");
                        response.SetHeader_NoCache();
                        response.SetResponse_OK();

                        if (m_request->get_method() == pion::http::types::REQUEST_METHOD_GET)
                        {
                            response.SetContent(std::string(
                                reinterpret_cast<char *>(&data[0]),
                                data.size()));
                        }

                        response.Send();
                    }
#else // RELEASE mode
                    Response response(*this);

                    response.SetHeader_ContentType(file_ptr->GetMimeType());
                    response.SetHeader("Content-Encoding", file_ptr->GetEncoding());
                    response.SetHeader("ETag", file_ptr->GetStrongETag());
                    response.SetHeader("Cache-Control", "public, max-age=3600");

                    if (m_request->get_header("If-None-Match") == file_ptr->GetStrongETag())
                    {
                        // It's not changed - just send back not modified
                        response.SetResponse_NotModified();
                    }
                    else
                    {
                        response.SetResponse_OK();

                        if (m_request->get_method() == pion::http::types::REQUEST_METHOD_GET)
                        {
                            response.SetContent_NoCopy(file_ptr->GetData(), file_ptr->GetSize());
                        }
                    }

                    response.Send();
#endif
                }
                else
                {
                    RespondWithMethodNotAllowedError("GET HEAD");
                }

                return true;
            }
            else // resource not found in resource set
            {
                return false;
            }
        }

        void Request::RespondWithResourceOrNotFoundError(const ResourceFileSet &resources)
        {
            if (!RespondWithResource(resources))
            {
                RespondWithNotFoundError();
            }
        }

        void Request::RespondWithResourceOrNotFoundError(const ResourceFileSet &resources, const std::string &resource_path)
        {
            if (!RespondWithResource(resources, resource_path))
            {
                RespondWithNotFoundError();
            }
        }

        void Request::RespondWithNotFoundError()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            Response(*this)
                .SetResponse_NotFound()
                .SetHeader_NoCache()
                .SetHeader_ContentType("text/plain")
                .SetContent("404 Not Found")
                .Send();
        }

        void Request::RespondWithBadRequestError()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            Response(*this)
                .SetResponse_BadRequest()
                .SetHeader_ContentType("text/plain")
                .SetContent("400 Bad Request")
                .Send();
        }

        void Request::RespondWithTemporaryRedirect(const std::string &location)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            Response(*this)
                .SetResponse_Found()
                .SetHeader_NoCache()
                .SetHeader("Location", location)
                .Send();
        }

        void Request::RespondWithServerError(const std::string &message)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            Response(*this)
                .SetResponse_ServerError()
                .SetHeader_NoCache()
                .SetHeader_ContentType("text/plain")
                .SetContent(bbox::Format("500 Internal Server Error: %s", message))
                .Send();
        }

        void Request::RespondWithMethodNotAllowedError(const std::string &allowed_methods)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_auto_failure->NotHandled());

            Response(*this)
                .SetResponse_MethodNotAllowed()
                .SetHeader_NoCache()
                .SetHeader_ContentType("text/plain")
                .SetContent(bbox::Format("405 Method Not Allowed: Allowed methods - \"%s\"", allowed_methods))
                .Send();
        }

    } // namespace bbox::http
} // namespace bbox
