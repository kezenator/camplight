/**
 * @file
 *
 * Header for the bbox::http::client::ClientRequest class.
 */

#ifndef __BBOX__HTTP__CLIENT__CLIENT_REQUEST_H__
#define __BBOX__HTTP__CLIENT__CLIENT_REQUEST_H__

#pragma once

#include <memory>
#include <functional>
#include <bbox/Error.h>
#include <bbox/DebugOutput.h>
#include <bbox/http/client/ClientResponse.h>
#include <boost/beast/http/verb.hpp>

namespace bbox
{
    namespace http
    {
        namespace client
        {
            // Forward declaration
            class HttpClient;

            /**
             * A copyable handle to a request object.
             */
            class ClientRequest
            {
                friend class HttpClient;

                ClientRequest();

            public:

                using CompleteCallback = std::function<void(const bbox::Error &error, const ClientResponse &response)>;
				using Method = boost::beast::http::verb;

                ClientRequest(HttpClient &client, const std::string &url);
                ClientRequest(const ClientRequest &) = default;
                ClientRequest(ClientRequest &&) = default;
                ClientRequest &operator =(const ClientRequest &) = default;
                ClientRequest &operator =(ClientRequest &&) = default;
                ~ClientRequest() = default;

				void PrintState(bbox::DebugOutput &out) const;

				void SetMethod(Method method);
                void SetContentType(const std::string &content_type) { SetHeader("Content-type", content_type); }

                void SetHeader(const std::string &header, const std::string &value);
                void SetHeader_BasicAuthentication(const std::string &user_name, const std::string &password);

                void SetContent(const std::string &content);

                void Send(CompleteCallback &&complete_callback);

            private:

                struct Pimpl;

                std::shared_ptr<Pimpl> m_pimpl;
            };

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__CLIENT__CLIENT_REQUEST_H__
