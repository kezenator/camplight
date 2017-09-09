/**
* @file
*
* Implementation for the bbox::rt::net::TcpEndpoint class.
*/

#include <bbox/rt/net/TcpEndpoint.h>

#include <sstream>

namespace bbox {
    namespace rt {
        namespace net {

            std::string TcpEndpoint::ToString() const
            {
                std::stringstream stream;

                stream << *this;

                return stream.str();
            }


            bool TcpEndpoint::FromString(const char *str, TcpEndpoint &result)
            {
                return FromString(std::string(str), result);
            }

            bool TcpEndpoint::FromString(const std::string &str, TcpEndpoint &result)
            {
                // First, split the string into host and port by the last colon

                size_t last_colon_pos = str.rfind(':');
                if (last_colon_pos == std::string::npos)
                    return false;

                std::string host_str = str.substr(0, last_colon_pos);
                std::string port_str = str.substr(last_colon_pos + 1);

                if (host_str.empty() || port_str.empty())
                    return false;

                // Convert the host from a string, ensuring the IPv4/IPv6 differences

                IpAddress host;

                if ((host_str.size() > 2)
                    && (host_str[0] == '[')
                    && (host_str[host_str.size() - 1] == ']'))
                {
                    // Must be IPv6

                    host_str = host_str.substr(1, host_str.size() - 2);

                    if (!IpAddress::FromString(host_str, host))
                        return false;

                    if (!host.is_v6())
                        return false;
                }
                else
                {
                    // Assume IPv4

                    if (!IpAddress::FromString(host_str, host))
                        return false;

                    if (!host.is_v4())
                        return false;
                }

                // Convert the port from a string

                unsigned short port;

                {
                    std::stringstream stream(port_str);
                    stream >> port;
                    if (!stream)
                        return false;

                    // Ensure no more in string

                    char c;
                    stream >> c;
                    if (stream)
                        return false;
                }

                // Combine and return success

                result = TcpEndpoint(host, port);
                return true;
            }

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox
