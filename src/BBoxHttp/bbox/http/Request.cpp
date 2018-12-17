/**
* @file
*
* Implementation for the bbox::http::Request class.
*/

#include <bbox/http/Request.h>
#include <bbox/http/Response.h>
#include <bbox/http/ResourceFileSet.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/server/details/Connection.h>

#include <bbox/Assert.h>
#include <bbox/FromString.h>
#include <bbox/Format.h>

#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>

#ifdef _DEBUG
#include <bbox/FileUtils.h>
#endif

namespace bbox {
    namespace http {


        Request::Pimpl::Pimpl(
						server::HttpServer &server,
						server::details::Connection *connection_ptr,
						RequestPtr &&request_ptr)
            : m_server(server)
            , m_connection_ptr(connection_ptr)
			, m_request_ptr(std::move(request_ptr))
            , m_handled(false)
        {
        }

        Request::Pimpl::~Pimpl()
        {
			if (!m_handled)
			{
				Response::ResponsePtr response = std::make_unique<Response::ResponseType>(
					Response::Status::not_found,
					m_request_ptr->version());

				response->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
				response->set(boost::beast::http::field::content_type, "text/plain");
				response->keep_alive(m_request_ptr->keep_alive());
				response->body() = "404 Not Found";

				m_connection_ptr->Send(std::move(response));
			}
        }

        void Request::Pimpl::SetHandled()
        {
            m_handled = true;
        }

        Request::Request(server::HttpServer &server,
                         server::details::Connection *connection_ptr,
						 RequestPtr &&request_ptr)
            : m_pimpl_ptr(std::make_shared<Pimpl>(server, connection_ptr, std::move(request_ptr)))
        {
        }

		bbox::rt::net::TcpEndpoint Request::GetLocalEndpoint() const
		{
			BBOX_ASSERT(*this);

			return m_pimpl_ptr->m_connection_ptr->GetLocalEndpoint();
		}

        bbox::rt::net::TcpEndpoint Request::GetRemoteEndpoint() const
        {
            BBOX_ASSERT(*this);

			return m_pimpl_ptr->m_connection_ptr->GetRemoteEndpoint();
        }

		std::string Request::GetHost() const
		{
			BBOX_ASSERT(*this);

			auto it = m_pimpl_ptr->m_request_ptr->find(boost::beast::http::field::host);

			if (it != m_pimpl_ptr->m_request_ptr->end())
				return it->value().to_string();

			return m_pimpl_ptr->m_connection_ptr->GetLocalEndpoint().GetAddress().ToString();
		}

		std::string Request::GetRootUrl() const
        {
			auto result = bbox::Format("http://%s", GetHost());

			auto port = GetLocalEndpoint().GetPort();

			if (port != 80)
				result += bbox::Format(":%d", port);
			
			return result;
		}

        std::string Request::GetResource() const
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			std::string result = m_pimpl_ptr->m_request_ptr->target().to_string();

			for (char ch : { '#' , '?' })
			{
				auto pos = result.find(ch);
				if (pos != std::string::npos)
					result = result.substr(0, pos);
			}

			return result;
        }

        bool Request::HasQuery(const std::string &param)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			// TODO
			return false;
            //return m_pimpl_ptr->m_request_ptr->has_query(param);
        }

        std::string Request::GetQuery(const std::string &param)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			// TODO
			return std::string();
            //return m_pimpl_ptr->m_request_ptr->get_query(param);
        }

        std::string Request::GetFullQueryString()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            // TODO
			return std::string();
			//return m_pimpl_ptr->m_request_ptr->get_query_string();
        }

        bool Request::HasHeader(const std::string &str)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			return m_pimpl_ptr->m_request_ptr->find(str) != m_pimpl_ptr->m_request_ptr->end();
        }

        std::string Request::GetHeader(const std::string &header)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());
            
            std::string result;

			auto iterator = m_pimpl_ptr->m_request_ptr->find(header);
			
			if (iterator != m_pimpl_ptr->m_request_ptr->end())
			{
                result = iterator->value().to_string();
            }
            
            return result;
        }

        Request::Method Request::GetMethod()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            return m_pimpl_ptr->m_request_ptr->method();
        }
        
		std::string Request::GetMethodString()
		{
			BBOX_ASSERT(*this);
			BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			return m_pimpl_ptr->m_request_ptr->method_string().to_string();
		}

		bool Request::CheckIsWebSocketUpgradeOrRespondWithError(const std::string &protocol)
		{
			BBOX_ASSERT(*this);
			BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			if (!boost::beast::websocket::is_upgrade(*m_pimpl_ptr->m_request_ptr))
			{
				RespondWithBadRequestError("WebSocket Upgrade request expected");
				return false;
			}

			auto it = m_pimpl_ptr->m_request_ptr->find(boost::beast::http::field::sec_websocket_protocol);
			if (it == m_pimpl_ptr->m_request_ptr->end())
			{
				RespondWithBadRequestError("WebSocket \"Sec-WebSocket-Protocol\" header expected");
				return false;
			}

			if (protocol != it->value())
			{
				RespondWithBadRequestError(bbox::Format("Only supports \"Sec-WebSocket-Protocol\" header \"%s\"", protocol));
				return false;
			}

			return true;
		}

        std::string Request::GetContent()
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

			return m_pimpl_ptr->m_request_ptr->body();
        }

        bool Request::RespondWithResource(const ResourceFileSet &resources)
        {
            return RespondWithResource(resources, GetResource());
        }

        bool Request::RespondWithResource(const ResourceFileSet &resources, const std::string &resource_path)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            const ResourceFile *file_ptr = resources.FindFile(resource_path);

            if (file_ptr)
            {
				Method method = m_pimpl_ptr->m_request_ptr->method();

                if ((method == Method::get)
					|| (method == Method::head))
                {
#ifdef _DEBUG
                    std::vector<uint8_t> data;
                    bbox::Error error = bbox::FileUtils::ReadBinaryFile(file_ptr->GetOrigFileName(), data, 250 * 1024 * 1024);

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

                        if (method == Method::get)
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

                    if (GetHeader("If-None-Match") == file_ptr->GetStrongETag())
                    {
                        // It's not changed - just send back not modified
                        response.SetResponse_NotModified();
                    }
                    else
                    {
                        response.SetResponse_OK();

                        if (method == Method::get)
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
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            Response(*this)
                .SetResponse_NotFound()
                .SetHeader_NoCache()
                .SetHeader_ContentType("text/plain")
                .SetContent("404 Not Found")
                .Send();
        }

        void Request::RespondWithBadRequestError(const std::string &error)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            Response(*this)
                .SetResponse_BadRequest()
                .SetHeader_ContentType("text/plain")
                .SetContent(bbox::Format("400 Bad Request: %s", error))
                .Send();
        }

        void Request::RespondWithTemporaryRedirect(const std::string &location)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            Response(*this)
                .SetResponse_Found()
                .SetHeader_NoCache()
                .SetHeader("Location", location)
                .Send();
        }

        void Request::RespondWithServerError(const std::string &message)
        {
            BBOX_ASSERT(*this);
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

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
            BBOX_ASSERT(m_pimpl_ptr->NotHandled());

            Response(*this)
                .SetResponse_MethodNotAllowed()
                .SetHeader_NoCache()
                .SetHeader_ContentType("text/plain")
                .SetContent(bbox::Format("405 Method Not Allowed: Allowed methods - \"%s\"", allowed_methods))
                .Send();
        }

    } // namespace bbox::http
} // namespace bbox
