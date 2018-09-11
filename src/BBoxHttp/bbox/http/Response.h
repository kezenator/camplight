/**
 * @file
 *
 * Header for the bbox::http::Response class.
 */

#ifndef __BBOX__RT__HTTP__RESPONSE_H__
#define __BBOX__RT__HTTP__RESPONSE_H__

#include <bbox/http/Request.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace bbox {
    namespace http {

        /**
         * Value that builds a response.
         */
        class Response
        {
			friend class server::details::Connection;
			friend class Request;

            Response() = delete;
            Response(const Response &) = delete;
            Response(Response &&) = delete;
            Response &operator =(const Response &) = delete;
            Response &operator =(Response &&) = delete;

        public:

			using Status = boost::beast::http::status;

            explicit Response(Request &request);
            ~Response();

            Response &SetResponse(Status status);

            Response &SetResponse_OK()
            {
				return SetResponse(Status::ok);
            }

            Response &SetResponse_NotModified()
            {
                return SetResponse(Status::not_modified);
            }

            Response &SetResponse_NotFound()
            {
                return SetResponse(Status::not_found);
            }

            Response &SetResponse_BadRequest()
            {
                return SetResponse(Status::bad_request);
            }

            Response &SetResponse_Found()
            {
                return SetResponse(Status::found);
            }

            Response &SetResponse_MethodNotAllowed()
            {
                return SetResponse(Status::method_not_allowed);
            }

            Response &SetResponse_ServerError()
            {
                return SetResponse(Status::internal_server_error);
            }

            Response &SetContent(const std::string &content);
            Response &SetContent_NoCopy(const void *data, size_t length);

            Response &SetHeader(const std::string &header, const std::string &value);

            Response &SetHeader_NoCache();
            Response &SetHeader_ContentType(const std::string &type);

            void Send();

        private:

			using ResponseType = boost::beast::http::response<boost::beast::http::string_body>;
			using ResponsePtr = std::unique_ptr<ResponseType>;

			Request m_request;
			ResponsePtr m_response_ptr;
        };

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__RESPONSE_H__