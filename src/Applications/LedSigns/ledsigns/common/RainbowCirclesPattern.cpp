/**
 * @file
 *
 * Implementation file for the ledsigns::common::RainbowCirclesPattern class.
 */

#include <ledsigns/common/RainbowCirclesPattern.h>
#include <leds/HsvColor.h>

#include <cmath>

namespace ledsigns
{
    namespace common
    {

        RainbowCirclesPattern::RainbowCirclesPattern(const common::RenderState &render, uint64_t period_ms, double width)
            : m_start_time_ms(render.time_ms)
            , m_period_ms(period_ms)
            , m_width(width)
        {
        }

        RainbowCirclesPattern::~RainbowCirclesPattern()
        {
        }

        std::string RainbowCirclesPattern::GetName() const
        {
            return bbox::Format("[Rainbow Circles, period = %d ms, width = %f]", m_period_ms, m_width);
        }
        
        void RainbowCirclesPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time : %d ms\n", m_start_time_ms);
            out.Format("Period     : %d ms\n", m_period_ms);
            out.Format("Width      : %f\n",    m_width);
        }

        std::vector<leds::Color> RainbowCirclesPattern::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                double lx = 2.0 * (render.layout.entries[i].x - 0.5);
                double ly = 2.0 * (render.layout.entries[i].y - 0.5);

                double dist = std::sqrt(lx*lx + ly*ly);
                
                uint64_t time_loop = (render.time_ms - m_start_time_ms) % m_period_ms;

                uint8_t hue = uint8_t((time_loop * 255) / m_period_ms);

                hue = uint64_t(hue + 256 - (255.0 * dist / m_width)) % 255;

                result[i] = leds::HsvColor(hue, 255, 128).ToRgb();
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
