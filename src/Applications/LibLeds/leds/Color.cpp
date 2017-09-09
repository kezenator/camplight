/**
 * @file
 *
 * Implementation file for the leds::Color class.
 */

#include <leds/Color.h>
#include <leds/HsvColor.h>
#include <bbox/Format.h>

// TODO - remove when implement ToHsv
#include <cassert>

namespace leds {

    std::string Color::ToString() const
    {
        return bbox::Format("#%02X%02X%02X", m_red, m_green, m_blue);
    }

    HsvColor Color::ToHsv() const
    {
        // TODO - not implemented
        assert(false);
        return HsvColor();
    }

    void Color::ToBinary(bbox::enc::ToBinary &m) const
    {
        m.Write(m_red);
        m.Write(m_green);
        m.Write(m_blue);
    }

    void Color::FromBinary(bbox::enc::FromBinary &m)
    {
        m.Read(m_red);
        m.Read(m_green);
        m.Read(m_blue);
    }

    void Color::ToTextFormat(bbox::enc::ToTextFormat &m) const
    {
        m.SetValue(ToString());
    }

    void Color::FromTextFormat(bbox::enc::FromTextFormat &m)
    {
        std::string str = m.DecodeStringValue();
        bool decoded = false;

        if ((str.size() == 7)
            && (str[0] == '#'))
        {
            std::string r_str = str.substr(1, 2);
            std::string g_str = str.substr(3, 2);
            std::string b_str = str.substr(5, 2);

            if (bbox::FromString(std::string("0x") + r_str, m_red)
                && bbox::FromString(std::string("0x") + g_str, m_green)
                && bbox::FromString(std::string("0x") + b_str, m_blue))
            {
                decoded = true;
            }
        }

        if (!decoded)
        {
            m.SetError("Expected camplight::render::Color string value of the format \"#rrggbb\"");
        }
    }

} // namespace leds