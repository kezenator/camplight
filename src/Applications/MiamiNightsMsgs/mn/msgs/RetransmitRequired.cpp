/**
 * @file
 *
 * Source for the mn::msgs::RetransmitRequired struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "mn_msgs.bbidl"
 */

#include <mn/msgs/RetransmitRequired.h>

namespace mn {
namespace msgs {

void RetransmitRequired::ToBinary(bbox::enc::ToBinary &m) const
{
    (void)m;
}

void RetransmitRequired::FromBinary(bbox::enc::FromBinary &m)
{
    (void)m;
}

void RetransmitRequired::ToTextFormat(bbox::enc::ToTextFormat &m) const
{
    m.StartStructure();
    m.CompleteStructure();
}

void RetransmitRequired::FromTextFormat(bbox::enc::FromTextFormat &m)
{
    m.StartStructure();
    m.CompleteStructure();
}

bbox::enc::MsgTypeLibrary::Registration<RetransmitRequired>
    RetransmitRequired::g_msg_registration("mn::msgs::RetransmitRequired", []() { return new_message<mn::msgs::RetransmitRequired>(); });

} // namespace msgs
} // namespace mn

