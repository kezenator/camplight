/**
* @file
*
* Implementation for the bbox::rt::net::AdapterInfo class.
*/

#include <bbox/rt/net/AdapterInfo.h>

namespace bbox {
    namespace rt {
        namespace net {

            AdapterInfo::AdapterInfo()
            {
            }

            AdapterInfo::AdapterInfo(const AdapterInfo &other)
                : system_name(other.system_name)
                , user_name(other.user_name)
                , description(other.description)
                , mac_address(other.mac_address)
                , ip_addresses(other.ip_addresses)
            {
            }

            AdapterInfo::AdapterInfo(AdapterInfo &&other)
                : system_name(std::move(other.system_name))
                , user_name(std::move(other.user_name))
                , description(std::move(other.description))
                , mac_address(std::move(other.mac_address))
                , ip_addresses(std::move(other.ip_addresses))
            {
            }

            AdapterInfo::~AdapterInfo()
            {
            }

            AdapterInfo &AdapterInfo::operator =(const AdapterInfo &other)
            {
                if (&other != this)
                {
                    system_name = other.system_name;
                    user_name = other.user_name;
                    description = other.description;
                    mac_address = other.mac_address;
                    ip_addresses = other.ip_addresses;
                }

                return *this;
            }

            AdapterInfo &AdapterInfo::operator =(AdapterInfo &&other)
            {
                if (&other != this)
                {
                    system_name = std::move(other.system_name);
                    user_name = std::move(other.user_name);
                    description = std::move(other.description);
                    mac_address = std::move(other.mac_address);
                    ip_addresses = std::move(other.ip_addresses);
                }

                return *this;
            }

            bool AdapterInfo::operator ==(const AdapterInfo &other) const
            {
                if (&other == this)
                    return true;

                if ((system_name == other.system_name)
                    && (user_name == other.user_name)
                    && (description == other.description)
                    && (mac_address == other.mac_address)
                    && (ip_addresses == other.ip_addresses))
                {
                    return true;
                }

                return false;
            }


        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox
