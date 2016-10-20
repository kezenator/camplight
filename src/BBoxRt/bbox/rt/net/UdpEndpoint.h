/**
 * @file
 *
 * Header for the bbox::rt::net::UdpEndpoint class.
 */

#ifndef __BBOX__RT__NET__UDP_ENDPOINT_H__
#define __BBOX__RT__NET__UDP_ENDPOINT_H__

#pragma once

#include <string>
#include <boost/asio/ip/udp.hpp>

#include <bbox/rt/net/IpAddress.h>

namespace bbox {
    namespace rt { 
        namespace net {

            /**
             * An UDP Endpoint.
             */
            class UdpEndpoint: public boost::asio::ip::udp::endpoint
            {
            public:
                UdpEndpoint()
                {
                }

                UdpEndpoint(const boost::asio::ip::address &address, unsigned short port)
                    : boost::asio::ip::udp::endpoint(address, port)
                {
                }

                UdpEndpoint(const boost::asio::ip::udp::endpoint &other)
                    : boost::asio::ip::udp::endpoint(other)
                {
                }

                UdpEndpoint(const UdpEndpoint &other)
                    : boost::asio::ip::udp::endpoint(other)
                {
                }

                ~UdpEndpoint()
                {
                }

                IpAddress GetAddress() const { return IpAddress(address()); }
                unsigned short GetPort() const { return port(); }

                UdpEndpoint &operator =(const boost::asio::ip::udp::endpoint &other) { boost::asio::ip::udp::endpoint::operator =(other); return *this; }

                bool operator ==(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this) == other; }
                bool operator !=(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this) != other; }
                bool operator  <(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this)  < other; }
                bool operator <=(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this) <= other; }
                bool operator  >(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this)  > other; }
                bool operator >=(const boost::asio::ip::udp::endpoint &other) const { return static_cast<const boost::asio::ip::udp::endpoint>(*this) >= other; }

                std::string ToString() const;

                static bool FromString(const char *str, UdpEndpoint &result);
                static bool FromString(const std::string &str, UdpEndpoint &result);
            };

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__UDP_ENDPOINT_H__
