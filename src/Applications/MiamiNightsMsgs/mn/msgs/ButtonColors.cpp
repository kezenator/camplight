/**
 * @file
 *
 * Source for the mn::msgs::ButtonColors struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "mn_msgs.bbidl"
 */

#include <mn/msgs/ButtonColors.h>

namespace mn {
namespace msgs {

void ButtonColors::ToBinary(bbox::enc::ToBinary &m) const
{
    m.Write(back_color);
    m.Write(play_color);
    m.Write(wash_color);
    m.Write(button_colors);
}

void ButtonColors::FromBinary(bbox::enc::FromBinary &m)
{
    m.Read(back_color);
    m.Read(play_color);
    m.Read(wash_color);
    m.Read(button_colors);
}

void ButtonColors::ToTextFormat(bbox::enc::ToTextFormat &m) const
{
    m.StartStructure();
    m.AddNamedValue("back_color", back_color);
    m.AddNamedValue("play_color", play_color);
    m.AddNamedValue("wash_color", wash_color);
    m.AddNamedValue("button_colors", button_colors);
    m.CompleteStructure();
}

void ButtonColors::FromTextFormat(bbox::enc::FromTextFormat &m)
{
    m.StartStructure();
    m.DecodeNamedValue("back_color", back_color);
    m.DecodeNamedValue("play_color", play_color);
    m.DecodeNamedValue("wash_color", wash_color);
    m.DecodeNamedValue("button_colors", button_colors);
    m.CompleteStructure();
}

bbox::enc::MsgTypeLibrary::Registration<ButtonColors>
    ButtonColors::g_msg_registration("mn::msgs::ButtonColors", []() { return new_message<mn::msgs::ButtonColors>(); });

} // namespace msgs
} // namespace mn

