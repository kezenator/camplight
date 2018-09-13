/**
 * @file
 *
 * Header for the mn::msgs::RetransmitRequired struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "mn_msgs.bbidl"
 */

#ifndef __mn_msgs_RetransmitRequired_H__
#define __mn_msgs_RetransmitRequired_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace mn {
namespace msgs {

class RetransmitRequired
{
public:
    RetransmitRequired() = default;
    ~RetransmitRequired() = default;
    RetransmitRequired(const RetransmitRequired&) = default;
    RetransmitRequired(RetransmitRequired&&) = default;
    RetransmitRequired &operator =(const RetransmitRequired&) = default;
    RetransmitRequired &operator =(RetransmitRequired&&) = default;

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
    void FromTextFormat(bbox::enc::FromTextFormat &m);

    static bbox::enc::MsgTypeLibrary::Registration<RetransmitRequired> g_msg_registration;
};

} // namespace msgs
} // namespace mn

#endif // __mn_msgs_RetransmitRequired_H__
