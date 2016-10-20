/**
 * @file
 *
 * Header for the bbox::http::Request class.
 */

#ifndef __BBOX__RT__HTTP__REQUEST_H__
#define __BBOX__RT__HTTP__REQUEST_H__

#pragma once

#include <pion/http/server.hpp>

namespace bbox {
    namespace http {

        // Forward declarations
        class ResourceFileSet;
        class Response;
        namespace server {
            class HttpServer;
        }

        /**
         * Value that represents a handle to a response.
         */
        class Request
        {
            friend class Response;
            friend class server::HttpServer;

        private:
            Request(server::HttpServer &server_service,
                    const pion::http::server_ptr &server,
                    const pion::http::request_ptr &request,
                    const pion::tcp::connection_ptr &connection);

            class AutoFailureHandler
            {
            public:
                AutoFailureHandler(const pion::http::server_ptr &server,
                                   const pion::http::request_ptr &request,
                                   const pion::tcp::connection_ptr &connection);
                ~AutoFailureHandler();

                void SetHandled();
                bool NotHandled() { return !m_handled; }

                const pion::http::server_ptr m_server;
                const pion::http::request_ptr m_request;
                const pion::tcp::connection_ptr m_connection;

            private:
                bool m_handled;
            };

            typedef std::shared_ptr<AutoFailureHandler> AutoFailureHandlerPtr;

        public:

            Request();
            Request(const Request &other);
            Request(Request &&other);
            ~Request();

            Request &operator =(const Request &other);
            Request &operator =(Request &&other);

            explicit operator bool() const
            {
                return m_auto_failure.operator bool();
            }

            bool operator !() const
            {
                return !m_auto_failure;
            }

            std::string GetRootUrl() const;

            std::string GetResource() const;

            bool HasHeader(const std::string &str);
            std::string GetHeader(const std::string &header);

            bool HasQuery(const std::string &param);
            std::string GetQuery(const std::string &param);
            std::string GetFullQueryString();

            std::string GetMethod();
            bool IsMethod_GetOrHead();
            bool IsMethod_Get();
            bool IsMethod_Head();
            bool IsMethod_Put();

            std::string GetContent();

            bool RespondWithResource(const ResourceFileSet &resources);
            void RespondWithResourceOrNotFoundError(const ResourceFileSet &resources);

            void RespondWithNotFoundError();
            void RespondWithBadRequestError();

            void RespondWithTemporaryRedirect(const std::string &location);

            void RespondWithServerError(const std::string &message);
            void RespondWithMethodNotAllowedError(const std::string &allowed_methods);

			void RespondWithDebugPage(const std::string &debug_root_path);

        private:
            server::HttpServer *m_server_service;
            pion::http::server_ptr m_server;
            pion::http::request_ptr m_request;
            pion::tcp::connection_ptr m_connection;
            AutoFailureHandlerPtr m_auto_failure;
        };

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__REQUEST_H__