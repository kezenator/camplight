/**
 * @file
 *
 * Header for the bbox::http::Response class.
 */

#ifndef __BBOX__RT__HTTP__RESPONSE_H__
#define __BBOX__RT__HTTP__RESPONSE_H__

#pragma once

#include <bbox/http/Request.h>

#include <pion/http/types.hpp>

namespace bbox {
    namespace http {

        /**
         * Value that builds a response.
         */
        class Response
        {
            Response() = delete;
            Response(const Response &) = delete;
            Response(Response &&) = delete;
            Response &operator =(const Response &) = delete;
            Response &operator =(Response &&) = delete;

        public:
            explicit Response(Request &request);
            ~Response();

            Response &SetLifecycleForceClose();

            Response &SetResponse(int code, const std::string &msg);

            Response &SetResponse_OK()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_OK,
                    pion::http::types::RESPONSE_MESSAGE_OK);
            }

            Response &SetResponse_NotModified()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_NOT_MODIFIED,
                    pion::http::types::RESPONSE_MESSAGE_NOT_MODIFIED);
            }

            Response &SetResponse_NotFound()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_NOT_FOUND,
                    pion::http::types::RESPONSE_MESSAGE_NOT_FOUND);
            }

            Response &SetResponse_BadRequest()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_BAD_REQUEST,
                    pion::http::types::RESPONSE_MESSAGE_BAD_REQUEST);
            }

            Response &SetResponse_Found()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_FOUND,
                    pion::http::types::RESPONSE_MESSAGE_FOUND);
            }

            Response &SetResponse_MethodNotAllowed()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_METHOD_NOT_ALLOWED,
                    pion::http::types::RESPONSE_MESSAGE_METHOD_NOT_ALLOWED);
            }

            Response &SetResponse_ServerError()
            {
                return SetResponse(
                    pion::http::types::RESPONSE_CODE_SERVER_ERROR,
                    pion::http::types::RESPONSE_MESSAGE_SERVER_ERROR);
            }

            Response &SetContent(const std::string &content);
            Response &SetContent_NoCopy(const void *data, size_t length);

            Response &SetHeader(const std::string &header, const std::string &value);

            Response &SetHeader_NoCache();
            Response &SetHeader_ContentType(const std::string &type);

            void Send();

        private:

            struct Pimpl;
            std::shared_ptr<Pimpl> m_pimpl;

        };

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__RESPONSE_H__