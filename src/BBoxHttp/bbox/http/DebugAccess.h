/**
 * @file
 *
 * Header for the bbox::http::DebugAccess class.
 */

#ifndef __BBOX__RT__HTTP__DEBUG_ACCESS_H__
#define __BBOX__RT__HTTP__DEBUG_ACCESS_H__

#pragma once

#include <bbox/http/Request.h>

namespace bbox
{
    namespace http
    {

        /**
         * Helper functions to run a simple web-site
         * that provides access to the debug system.
         */
        class DebugAccess
        {
        public:
            static void HandleDebugRequest(Request &request, const std::string &debug_root_path);
            static std::string HtmlEscape(std::string &&str);
            static std::string UriEscape(std::string &&str);
        };

    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__HTTP__DEBUG_ACCESS_H__
