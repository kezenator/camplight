/**
 * @file
 *
 * Header for the bbox::http::server::RequestHandler class.
 */

#ifndef __BBOX__RT__HTTP__SERVER__REQUEST_HANDLER_H__
#define __BBOX__RT__HTTP__SERVER__REQUEST_HANDLER_H__

#pragma once

#include <bbox/rt/Resource.h>
#include <bbox/rt/DebugEnable.h>
#include <bbox/http/Request.h>
#include <bbox/http/Response.h>

namespace bbox
{
    namespace http
    {
        namespace server
        {

            // Forward declarations
            class HttpServer;

            /**
             * A login for a request handler for a HTTP server..
             */
            class RequestHandler : public bbox::rt::Resource
            {
                friend class HttpServer;

                RequestHandler() = delete;
                RequestHandler(const RequestHandler &) = delete;
                RequestHandler operator =(const RequestHandler &) = delete;

            public:

                typedef boost::function<void(Request &request)> HandlerFunc;

                RequestHandler(const std::string &name, rt::Service &parent, HttpServer &server);
                ~RequestHandler();

                void Login(std::string &&prefix, HandlerFunc &&handler);
                void Logout();

            private:
                void HandleStopping() override;
                void PrintState(bbox::DebugOutput &out) const override;

                HttpServer &m_server;
                bool m_is_logged_in;
                std::string m_prefix;
                HandlerFunc m_handler_func;
                bbox::rt::DebugEnable m_debug_enable;
            };

        } // namespace bbox::http::server
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__SERVER__REQUEST_HANDLER_H__
