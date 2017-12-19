/**
 * @file
 *
 * Implementation file for the ledsigns::common::GrowingCirclesPattern class.
 */

#include <ledsigns/common/GrowingCirclesPattern.h>
#include <leds/HsvColor.h>

#include <cmath>

namespace ledsigns
{
    namespace common
    {

        GrowingCirclesPattern::GrowingCirclesPattern(const common::RenderState &render, uint64_t period_ms, double width, Gradient::Ptr gradient)
            : m_start_time_ms(render.time_ms)
            , m_period_ms(period_ms)
            , m_width(width)
            , m_gradient(gradient)
        {
        }

        GrowingCirclesPattern::~GrowingCirclesPattern()
        {
        }

        std::string GrowingCirclesPattern::GetName() const
        {
            return bbox::Format("[Growing Circles, period = %d ms, width = %f, gradient = %s]", m_period_ms, m_width, m_gradient->GetName());
        }
        
        void GrowingCirclesPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time : %d ms\n", m_start_time_ms);
            out.Format("Period     : %d ms\n", m_period_ms);
            out.Format("Width      : %f\n",    m_width);
            out.Format("Graident   : %s\n", m_gradient->GetName());
            m_gradient->PrintInformation(out);
        }

        std::vector<leds::Color> GrowingCirclesPattern::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                double lx = 2.0 * (render.layout.entries[i].x - 0.5);
                double ly = 2.0 * (render.layout.entries[i].y - 0.5);

                double dist = std::sqrt(lx*lx + ly*ly);
                
                uint64_t time_loop = (render.time_ms - m_start_time_ms) % m_period_ms;

                uint8_t grad_progress = uint8_t((time_loop * 255) / m_period_ms);

                grad_progress = uint64_t(grad_progress + 256 - (255.0 * dist / m_width)) % 255;

                result[i] = m_gradient->Convert(grad_progress / 256.0);
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
