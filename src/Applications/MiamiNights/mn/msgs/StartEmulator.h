/**
 * @file
 *
 * Header for the mn::msgs::StartEmulator struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "mn_msgs.bbidl"
 */

#ifndef __mn_msgs_StartEmulator_H__
#define __mn_msgs_StartEmulator_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace mn {
namespace msgs {

class StartEmulator
{
public:
    StartEmulator() = default;
    ~StartEmulator() = default;
    StartEmulator(const StartEmulator&) = default;
    StartEmulator(StartEmulator&&) = default;
    StartEmulator &operator =(const StartEmulator&) = default;
    StartEmulator &operator =(StartEmulator&&) = default;

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
    void FromTextFormat(bbox::enc::FromTextFormat &m);

    std::string game;

    static bbox::enc::MsgTypeLibrary::Registration<StartEmulator> g_msg_registration;
};

} // namespace msgs
} // namespace mn

#endif // __mn_msgs_StartEmulator_H__
