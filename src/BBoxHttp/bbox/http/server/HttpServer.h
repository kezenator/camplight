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

namespace bbox {
    namespace http {

        // Forward declarations
		class Request;
        class Response;

        namespace server {

            // Forward declarations
			class Connection;
			class RequestHandler;

            /**
             * Implements a HTTP server.
             */
            class HttpServer : public rt::Service
            {
                friend class ::bbox::http::server::RequestHandler;
				friend class ::bbox::http::server::Connection;

            public:

                using HandlerFunc = std::function<void(Request &request)>;

                HttpServer(const std::string &name, rt::Service &parent);
                virtual ~HttpServer();

                bool AddServer(const rt::net::TcpEndpoint &tcp_endpoint,
                               HandlerFunc default_handler = HandlerFunc());

                unsigned short AddUnassignedPortServer(const rt::net::IpAddress &ip_address,
                                                       HandlerFunc default_handler = HandlerFunc());

                void TryAndOpenWebBrowserToServer(const std::string &path = "/");

            private:
				struct RequestHandlerOrder
				{
					bool operator()(RequestHandler *a, RequestHandler *b) const;
				};

				struct Listener;
				
				void HandleStarting() override;
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;

                void CheckShutdown();

                void ListenerStopped(Listener *listener);
				void ConnectionClosed(Connection *connection);
                void HandleRequest(Request &&request, const HandlerFunc &server_handler);

                rt::OneShotWork m_check_shutdown_work;
				std::map<Listener *, std::unique_ptr<Listener>> m_listeners;
				std::map<Connection *, std::unique_ptr<Connection>> m_connections;
				uint64_t m_num_requests_received;

                std::set<RequestHandler *, RequestHandlerOrder> m_request_handlers;

                bbox::rt::DebugEnable m_default_debug_enable;
            };

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__SERVER__HTTP_SERVER_H__
