/**
 * @file
 *
 * Header for the bbox::http::debug::msgs::EnableRequest struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_http_debug_msgs.bbidl"
 */

#ifndef __bbox_http_debug_msgs_EnableRequest_H__
#define __bbox_http_debug_msgs_EnableRequest_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace bbox {
namespace http {
namespace debug {
namespace msgs {

class EnableRequest
{
public:
    EnableRequest() = default;
    ~EnableRequest() = default;
    EnableRequest(const EnableRequest&) = default;
    EnableRequest(EnableRequest&&) = default;
    EnableRequest &operator =(const EnableRequest&) = default;
    EnableRequest &operator =(EnableRequest&&) = default;

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
    void FromTextFormat(bbox::enc::FromTextFormat &m);

    bool operator ==(const EnableRequest &other) const
    {
        return (enabled == other.enabled)
            && (debug_enables == other.debug_enables);
    }

    bool operator !=(const EnableRequest &other) const
    {
        return (enabled != other.enabled)
            || (debug_enables != other.debug_enables);
    }

    bool enabled;
    std::vector<std::string> debug_enables;

    static bbox::enc::MsgTypeLibrary::Registration<EnableRequest> g_msg_registration;
};

} // namespace msgs
} // namespace debug
} // namespace http
} // namespace bbox

#endif // __bbox_http_debug_msgs_EnableRequest_H__
