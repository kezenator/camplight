/**
 * @file
 *
 * Header file for the camplight::render::Color class.
 */

#ifndef __CAMPLIGHT__RENDER__COLOR_H__
#define __CAMPLIGHT__RENDER__COLOR_H__

#include <string>
#include <cstdint>
#include <iostream>

#include <bbox/enc/ToBinary.h>
#include <bbox/enc/FromBinary.h>
#include <bbox/enc/ToTextFormat.h>
#include <bbox/enc/FromTextFormat.h>

namespace camplight {
    namespace render {

        // Forward declaration
        class HsvColor;

        class Color
        {
        public:
            Color()
                : m_red(0), m_green(0), m_blue(0)
            {
            }

            Color(uint8_t red, uint8_t green, uint8_t blue)
                : m_red(red), m_green(green), m_blue(blue)
            {
            }

            Color(const Color &other)
                : m_red(other.m_red), m_green(other.m_green), m_blue(other.m_blue)
            {
            }

            ~Color()
            {
            }

            const Color &operator =(const Color &other)
            {
                m_red = other.m_red;
                m_green = other.m_green;
                m_blue = other.m_blue;
                return *this;
            }

            std::string ToString() const;

            HsvColor ToHsv() const;

            uint32_t ToWs2811Color() const
            {
                return m_blue
                    | (uint32_t(m_green) << 8)
                    | (uint32_t(m_red) << 16);
            }

            uint8_t GetRed() const { return m_red; }
            uint8_t GetGreen() const { return m_green; }
            uint8_t GetBlue() const { return m_blue; }

            void ToBinary(bbox::enc::ToBinary &m) const;
            void FromBinary(bbox::enc::FromBinary &m);
            void ToTextFormat(bbox::enc::ToTextFormat &m) const;
            void FromTextFormat(bbox::enc::FromTextFormat &m);

            Color operator +(const Color &other) const { return Color(add_sat(m_red, other.m_red), add_sat(m_green, other.m_green), add_sat(m_blue, other.m_blue)); }
            const Color &operator +=(const Color &other) { m_red = add_sat(m_red, other.m_red); m_green = add_sat(m_green, other.m_green); m_blue = add_sat(m_blue, other.m_blue); return *this; }

            Color Scale(uint8_t range) const { return Color(scale(m_red, range), scale(m_green, range), scale(m_blue, range)); }

            bool operator ==(const Color &other) const { return (m_red == other.m_red) && (m_green == other.m_green) && (m_blue == other.m_blue); }

        private:

            static uint8_t add_sat(uint8_t a, uint8_t b)
            {
                uint16_t sum = a + b;
                if (sum > 0xFF)
                    sum = 0xFF;
                return uint8_t(sum);
            }

            static uint8_t scale(uint8_t val, uint8_t range)
            {
                return uint8_t((val * range) / 255);
            }

            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
        };

        inline std::ostream &operator <<(std::ostream &stream, const Color &color)
        {
            stream << color.ToString();
            return stream;
        }
        
    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__COLOR_H__
