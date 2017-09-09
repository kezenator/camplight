/**
 * @file
 *
 * Header for the bbox::rt::net::TcpEndpoint class.
 */

#ifndef __BBOX__RT__NET__TCP_ENDPOINT_H__
#define __BBOX__RT__NET__TCP_ENDPOINT_H__

#pragma once

#include <string>
#include <boost/asio/ip/tcp.hpp>

#include <bbox/rt/net/IpAddress.h>

namespace bbox {
    namespace rt { 
        namespace net {

            /**
             * An UDP Endpoint.
             */
            class TcpEndpoint: public boost::asio::ip::tcp::endpoint
            {
            public:
                TcpEndpoint()
                {
                }

                TcpEndpoint(const boost::asio::ip::address &address, unsigned short port)
                    : boost::asio::ip::tcp::endpoint(address, port)
                {
                }

                TcpEndpoint(const boost::asio::ip::tcp::endpoint &other)
                    : boost::asio::ip::tcp::endpoint(other)
                {
                }

                TcpEndpoint(const TcpEndpoint &other)
                    : boost::asio::ip::tcp::endpoint(other)
                {
                }

                ~TcpEndpoint()
                {
                }

                IpAddress GetAddress() const { return IpAddress(address()); }
                unsigned short GetPort() const { return port(); }

                TcpEndpoint &operator =(const boost::asio::ip::tcp::endpoint &other) { boost::asio::ip::tcp::endpoint::operator =(other); return *this; }

                bool operator ==(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this) == other; }
                bool operator !=(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this) != other; }
                bool operator  <(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this)  < other; }
                bool operator <=(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this) <= other; }
                bool operator  >(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this)  > other; }
                bool operator >=(const boost::asio::ip::tcp::endpoint &other) const { return static_cast<const boost::asio::ip::tcp::endpoint>(*this) >= other; }

                std::string ToString() const;

                static bool FromString(const char *str, TcpEndpoint &result);
                static bool FromString(const std::string &str, TcpEndpoint &result);
            };

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__TCP_ENDPOINT_H__
