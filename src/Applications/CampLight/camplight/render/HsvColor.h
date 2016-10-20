/**
 * @file
 *
 * Header file for the camplight::render::HsvColor class.
 */

#ifndef __CAMPLIGHT__RENDER__HSV_COLOR_H__
#define __CAMPLIGHT__RENDER__HSV_COLOR_H__

#include <string>
#include <cstdint>
#include <iostream>

namespace camplight {
    namespace render {

        // Forward declaration
        class Color;

        class HsvColor
        {
        public:
            HsvColor()
                : m_hue(0), m_sat(0), m_val(0)
            {
            }

            HsvColor(uint8_t hue, uint8_t sat, uint8_t val)
                : m_hue(hue), m_sat(sat), m_val(val)
            {
            }

            HsvColor(const HsvColor &other)
                : m_hue(other.m_hue), m_sat(other.m_sat), m_val(other.m_val)
            {
            }

            ~HsvColor()
            {
            }

            const HsvColor &operator =(const HsvColor &other)
            {
                m_hue = other.m_hue;
                m_sat = other.m_sat;
                m_val = other.m_val;
                return *this;
            }

            std::string ToString() const;

            Color ToRgb() const;

        private:
            uint8_t m_hue;
            uint8_t m_sat;
            uint8_t m_val;
        };

        inline std::ostream &operator <<(std::ostream &stream, const HsvColor &color)
        {
            stream << color.ToString();
            return stream;
        }

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__HSV_COLOR_H__
