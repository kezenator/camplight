/**
 * @file
 *
 * Header for the bbox::http::server::HttpServer class.
 */

#ifndef __BBOX__RT__HTTP__SERVER__HTTP_SERVER_H__
#define __BBOX__RT__HTTP__SERVER__HTTP_SERVER_H__

#pragma once

#include <bbox/rt/Service.h>
#include <bbox/rt/DebugEnable.h>
#include <bbox/rt/net/TcpEndpoint.h>
#include <bbox/rt/OneShotWork.h>

#include <bbox/http/Request.h>

#include <bbox/http/pionhlp/Scheduler.h>
#include <pion/http/server.hpp>

namespace bbox {
    namespace http {

        // Forward declarations
        class Response;

        namespace server {

            // Forward declarations
            class RequestHandler;

            /**
             * Implements a HTTP server.
             */
            class HttpServer : public rt::Service
            {
                friend class ::bbox::http::Response;
                friend class ::bbox::http::server::RequestHandler;

            public:

                typedef boost::function<void(Request &request)> HandlerFunc;

                HttpServer(const std::string &name, rt::Service &parent);
                virtual ~HttpServer();

                bool AddServer(const rt::net::TcpEndpoint &tcp_endpoint,
                               HandlerFunc default_handler = HandlerFunc());

                unsigned short AddUnassignedPortServer(const rt::net::IpAddress &ip_address,
                                                       HandlerFunc default_handler = HandlerFunc());

                void TryAndOpenWebBrowserToServer(const std::string &path = "/");

            private:
                void HandleStarting() override;
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;

                void CheckShutdown();

                void ServerStopped(pion::http::server_ptr &server);
                void HandleRequest(const pion::http::server_ptr &server,
                                   const pion::http::request_ptr &http_request_ptr,
                                   const pion::tcp::connection_ptr &tcp_conn,
                                   const HandlerFunc &user_handler);
                void NotifyRequestCompleted();

                struct RequestHandlerOrder
                {
                    bool operator()(RequestHandler *a, RequestHandler *b) const;
                };

                rt::OneShotWork m_check_shutdown_work;
                pionhlp::Scheduler m_scheduler;
                std::list<pion::http::server_ptr> m_servers;
				uint64_t m_num_requests_received;
                size_t m_num_outstanding_requests;

                std::set<RequestHandler *, RequestHandlerOrder> m_request_handlers;

                bbox::rt::DebugEnable m_default_debug_enable;
            };

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__SERVER__HTTP_SERVER_H__
