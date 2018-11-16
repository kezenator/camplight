/**
* @file
*
* Implementation for the bbox::rt::net::IpAddress class.
*/

#include <bbox/rt/net/IpAddress.h>

namespace bbox {
namespace rt {
namespace net {

bool IpAddress::FromString(const char *str, IpAddress &result)
{
    boost::system::error_code ec;

    result = boost::asio::ip::address::from_string(str, ec);

    return !ec;
}

bool IpAddress::FromString(const std::string &str, IpAddress &result)
{
    boost::system::error_code ec;

    result = boost::asio::ip::address::from_string(str, ec);

    return !ec;
}

void IpAddress::ToBinary(bbox::enc::ToBinary &m) const
{
    m.WriteString(ToString());
}

void IpAddress::FromBinary(bbox::enc::FromBinary &m)
{
    std::string str = m.ReadString();

    if (!FromString(str, *this))
    {
        m.SetError(bbox::Format("Invalid IPAddress string \"%s\"", str));
    }
}


} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
