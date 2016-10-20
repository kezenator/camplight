/**
 * @file
 *
 * Header for the bbox::http::client::ClientResponse class.
 */

#ifndef __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__
#define __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__

#pragma once

#include <memory>
#include <pion/http/response.hpp>

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

                int GetStatusCode() const { return m_response_ptr->get_status_code(); }
                const std::string &GetStatusMessage() const { return m_response_ptr->get_status_message(); }

                size_t GetContentLength() const { return m_response_ptr->get_content_length(); }
                std::string GetContentAsString() const { return std::string(m_response_ptr->get_content(), m_response_ptr->get_content_length()); }

            private:

                explicit ClientResponse(const pion::http::response_ptr &ptr);

                pion::http::response_ptr m_response_ptr;
            };

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__CLIENT__CLIENT_RESPONSE_H__
