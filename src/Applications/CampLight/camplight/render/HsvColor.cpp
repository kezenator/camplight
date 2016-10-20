/**
 * @file
 *
 * Implementation file for the camplight::render::HsvColor class.
 */

#include <camplight/render/Color.h>
#include <camplight/render/HsvColor.h>
#include <bbox/Format.h>
#include <cmath>

namespace camplight {
    namespace render {

        std::string
        HsvColor::ToString() const
        {
            return bbox::Format("HSV(%d,%d,%d)", m_hue, m_sat, m_val);
        }

        Color
        HsvColor::ToRgb() const
        {
            float h = float(m_hue) * (360.0f / 256.0f);
            float s = float(m_sat) / 255.0f;
            float v = float(m_val) / 255.0f;

            // Taken from:
            // http://www.cs.rit.edu/~ncs/color/t_convert.html
            // on 6th March 2015

            int i;
            float f, p, q, t;
            float r, g, b;

            if (s == 0)
            {
                // achromatic (grey)
                r = g = b = v;
            }
            else
            {
                h /= 60;            // sector 0 to 5
                i = int(h);
                f = h - i;          // factorial part of h
                p = v * (1 - s);
                q = v * (1 - s * f);
                t = v * (1 - s * (1 - f));

                switch (i)
                {
                case 0:
                    r = v;
                    g = t;
                    b = p;
                    break;
                case 1:
                    r = q;
                    g = v;
                    b = p;
                    break;
                case 2:
                    r = p;
                    g = v;
                    b = t;
                    break;
                case 3:
                    r = p;
                    g = q;
                    b = v;
                    break;
                case 4:
                    r = t;
                    g = p;
                    b = v;
                    break;
                default:		// case 5:
                    r = v;
                    g = p;
                    b = q;
                    break;
                }
            }

            return Color(uint8_t((r * 255.0) + 0.5),
                         uint8_t((g * 255.0) + 0.5),
                         uint8_t((b * 255.0) + 0.5));
        }

    } // namespace camplight::render
} // namespace camplight
