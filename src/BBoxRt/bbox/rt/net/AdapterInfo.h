/**
 * @file
 *
 * Header for the bbox::rt::net::AdapterInfo class.
 */

#ifndef __BBOX__RT__NET__ADAPTER_INFO_H__
#define __BBOX__RT__NET__ADAPTER_INFO_H__

#pragma once

#include <string>
#include <set>

namespace bbox {
    namespace rt { 
        namespace net {

            /**
             * Information about a network adapter on the system.
             */
            class AdapterInfo
            {
            public:
                AdapterInfo();
                AdapterInfo(const AdapterInfo &other);
                AdapterInfo(AdapterInfo &&other);
                ~AdapterInfo();
                AdapterInfo &operator =(const AdapterInfo &other);
                AdapterInfo &operator =(AdapterInfo &&other);

                bool operator ==(const AdapterInfo &other) const;
                bool operator !=(const AdapterInfo &other) const { return !operator==(other); }

                std::string system_name;
                std::string user_name;
                std::string description;
                std::string mac_address;
                std::set<std::string> ip_addresses;
            };

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__ADAPTER_INFO_H__