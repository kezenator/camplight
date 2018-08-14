/**
 * @file
 *
 * Header for the bbox::http::client::ClientResponse class.
 */

#ifndef __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__
#define __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__

#pragma once

#include <memory>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace bbox
{
    namespace http
    {
        namespace client
        {
            // Forward declaration
            class ClientRequest;

            /**
             * Information about a response.
             */
            class ClientResponse
            {
                friend class ClientRequest;

                ClientResponse() = delete;

            public:

                ClientResponse(const ClientResponse &) = default;
                ClientResponse(ClientResponse &&) = default;
                ClientResponse &operator =(const ClientResponse &) = default;
                ClientResponse &operator =(ClientResponse &&) = default;
                ~ClientResponse() = default;

                int GetStatusCode() const { return int(m_response_ptr->result_int()); }
                std::string GetStatusMessage() const { return m_response_ptr->reason().to_string(); }

                size_t GetContentLength() const { return m_response_ptr->body().size(); }
                std::string GetContentAsString() const { return m_response_ptr->body(); }

            private:

                explicit ClientResponse(std::unique_ptr<boost::beast::http::response<boost::beast::http::string_body>> &&ptr);

                std::unique_ptr<boost::beast::http::response<boost::beast::http::string_body>> m_response_ptr;
            };

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__
