/**
* @file
*
* Implementation for the bbox::http::Request class.
*/

#include <bbox/http/Response.h>
#include <bbox/http/server/Connection.h>

#include <bbox/Assert.h>

#include <boost/beast/version.hpp>

namespace bbox {
    namespace http {

        Response::Response(Request &request)
			: m_request(request)
			, m_response_ptr(std::make_unique<ResponseType>(Status::internal_server_error, m_request.m_pimpl_ptr->m_request_ptr->version()))
        {
			m_response_ptr->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
			m_response_ptr->keep_alive(m_request.m_pimpl_ptr->m_request_ptr->keep_alive());
		}

        Response::~Response()
        {
        }

        Response &Response::SetResponse(Status status)
        {
            BBOX_ASSERT(m_response_ptr);

			m_response_ptr->result(status);

            return *this;
        }

        Response &Response::SetContent(const std::string &content)
        {
            BBOX_ASSERT(m_response_ptr);

			m_response_ptr->body().assign(content);
			m_response_ptr->content_length(content.size());

            return *this;
        }

        Response &Response::SetContent_NoCopy(const void *data, size_t length)
        {
			return SetContent(std::string(reinterpret_cast<const char *>(data), length));
        }

        Response &Response::SetHeader(const std::string &header, const std::string &value)
        {
            BBOX_ASSERT(m_response_ptr);

			m_response_ptr->set(header, value);

            return *this;
        }

        Response &Response::SetHeader_NoCache()
        {
            BBOX_ASSERT(m_response_ptr);

			m_response_ptr->set(
				boost::beast::http::field::cache_control,
				"private, max-age=0, no-cache, no-store");

            return *this;
        }

        Response &Response::SetHeader_ContentType(const std::string &type)
        {
            BBOX_ASSERT(m_response_ptr);

			m_response_ptr->set(
				boost::beast::http::field::content_type,
				type);

            return *this;
        }
        
        void Response::Send()
        {
			BBOX_ASSERT(m_request);
            BBOX_ASSERT(m_response_ptr);
            BBOX_ASSERT(m_request.m_pimpl_ptr->NotHandled());

			{
				ResponsePtr response = std::move(m_response_ptr);
				m_response_ptr.reset();

				m_request.m_pimpl_ptr->SetHandled();
				m_request.m_pimpl_ptr->m_connection_ptr->Send(std::move(response));
			}
        }

    } // namespace bbox::http
} // namespace bbox
