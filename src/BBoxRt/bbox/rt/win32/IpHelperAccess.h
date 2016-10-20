/**
 * @file
 *
 * Header for the bbox::rt::win32::IpHelperAccess class.
 */

#ifndef __BBOX__RT__WIN32__IP_HELPER_ACCESS_H__
#define __BBOX__RT__WIN32__IP_HELPER_ACCESS_H__

#pragma once

#include <map>
#include <string>

#include <bbox/Error.h>
#include <bbox/rt/net/AdapterInfo.h>

namespace bbox {
    namespace rt { 
        namespace win32 {

            /**
             * A class that provides access to the IP Helper Win32 APIs
             * because the boost headers muck up including IpHlpApi.h
             * directly in compilation units that include boost headers.
             */
            class IpHelperAccess
            {
            public:
                static bbox::Error RegisterChangeNotifier(void *&notifier, void(*handler)(void *context), void *context);
                static bbox::Error DestroyChangeNotifier(void *&notifier);

                static bbox::Error QueryAdapters(std::map<std::string, net::AdapterInfo> &results);
            };

        } // namespae bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__WIN32__IP_HELPER_ACCESS_H__