/**
 * @file
 *
 * Header for the bbox::http::client::HttpClient class.
 */

#ifndef __BBOX__HTTP__CLIENT__HTTP_CLIENT_H__
#define __BBOX__HTTP__CLIENT__HTTP_CLIENT_H__

#pragma once

#include <memory>
#include <bbox/rt/Resource.h>

namespace bbox
{
    namespace http
    {
        namespace client
        {
            // Forward declarations
            class ClientRequest;
            class ClientResponse;

            /**
             * Implements a HTTP client.
             */
            class HttpClient : public rt::Resource
            {
                friend class ClientRequest;

            public:

                HttpClient(const std::string &name, rt::Resource &parent);
                HttpClient(const std::string &name, rt::Service &parent);
                ~HttpClient();

            private:
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;
                void CheckStopped();

                void AddNewRequest(const ClientRequest &request);
                void RemoveCompletedRequest(const ClientRequest &request);

                struct Pimpl;

                std::unique_ptr<Pimpl> m_pimpl;
            };

        } // namespace bbox::http::client
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__CLIENT__HTTP_CLIENT_H__
