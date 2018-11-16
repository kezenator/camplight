/**
 * @file
 *
 * Header for the bbox::rt::net::IpAddress class.
 */

#ifndef __BBOX__RT__NET__IP_ADDRESS_H__
#define __BBOX__RT__NET__IP_ADDRESS_H__

#pragma once

#include <string>
#include <boost/asio/ip/address.hpp>
#include <bbox/enc/FromBinary.h>
#include <bbox/enc/ToBinary.h>

namespace bbox {
namespace rt {
namespace net {

/**
 * An IP address.
 */
class IpAddress : public boost::asio::ip::address
{
public:
    IpAddress()
    {
    }

    IpAddress(const boost::asio::ip::address &other)
        : boost::asio::ip::address(other)
    {
    }

    IpAddress(const boost::asio::ip::address_v4 &other)
        : boost::asio::ip::address(other)
    {
    }

    IpAddress(const boost::asio::ip::address_v6 &other)
        : boost::asio::ip::address(other)
    {
    }

    IpAddress(const IpAddress &other)
        : boost::asio::ip::address(other)
    {
    }

    ~IpAddress()
    {
    }

    IpAddress &operator =(const boost::asio::ip::address &other) { boost::asio::ip::address::operator =(other); return *this; }

    bool operator ==(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) == other; }
    bool operator !=(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) != other; }
    bool operator  <(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) < other; }
    bool operator <=(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) <= other; }
    bool operator  >(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) > other; }
    bool operator >=(const boost::asio::ip::address &other) const { return static_cast<const boost::asio::ip::address>(*this) >= other; }

    std::string ToString() const
    {
        return to_string();
    }

    static bool FromString(const char *str, IpAddress &result);
    static bool FromString(const std::string &str, IpAddress &result);

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
};

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__IP_ADDRESS_H__