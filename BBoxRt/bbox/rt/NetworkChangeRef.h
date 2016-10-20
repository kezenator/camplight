/**
 * @file
 *
 * Header for the bbox::rt::win32::NetworkChangeRef class.
 */

#ifndef __BBOX__RT__WIN32__NETWORK_CHANGE_REF_H__
#define __BBOX__RT__WIN32__NETWORK_CHANGE_REF_H__

#ifdef WIN32

#pragma once

#include <bbox/rt/ServiceReference.h>
#include <bbox/rt/win32/NetworkChangeService.h>

namespace bbox {
    namespace rt {

        /**
         * References the network change service
         * to get changes to network interfaces.
         */
        class NetworkChangeRef : public ServiceReference<win32::NetworkChangeService>
        {
        public:
            NetworkChangeRef(const std::string &name, Service &parent);
            ~NetworkChangeRef();

            std::map<std::string, net::AdapterInfo> GetCurrentAdapterInfo();
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // WIN32
#endif // __BBOX__RT__WIN32__NETWORK_CHANGE_REF_H__