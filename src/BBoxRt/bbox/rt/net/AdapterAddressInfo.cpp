/**
 * @file
 *
 * Source for the bbox::rt::net::AdapterAddressInfo struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_rt_net.bbidl"
 */

#include <bbox/rt/net/AdapterAddressInfo.h>

namespace bbox {
namespace rt {
namespace net {

void AdapterAddressInfo::ToBinary(bbox::enc::ToBinary &m) const
{
    m.Write(address);
    m.Write(broadcast);
}

void AdapterAddressInfo::FromBinary(bbox::enc::FromBinary &m)
{
    m.Read(address);
    m.Read(broadcast);
}

void AdapterAddressInfo::ToTextFormat(bbox::enc::ToTextFormat &m) const
{
    m.StartStructure();
    m.AddNamedValue("address", address);
    m.AddNamedValue("broadcast", broadcast);
    m.CompleteStructure();
}

void AdapterAddressInfo::FromTextFormat(bbox::enc::FromTextFormat &m)
{
    m.StartStructure();
    m.DecodeNamedValue("address", address);
    m.DecodeNamedValue("broadcast", broadcast);
    m.CompleteStructure();
}

} // namespace net
} // namespace rt
} // namespace bbox

