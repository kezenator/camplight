/**
 * @file
 *
 * Implementation file for the ledsigns::common::WavePattern class.
 */

#include <ledsigns/common/WavePattern.h>

#include <cmath>

namespace ledsigns
{
    namespace common
    {

        WavePattern::WavePattern(const common::RenderState &render, uint64_t period_ms, double width, Gradient::Ptr gradient)
            : m_start_time_ms(render.time_ms)
            , m_period_ms(period_ms)
            , m_width(width)
            , m_gradient(std::move(gradient))
            , m_angle((rand() % 360) * 6.283185307179586476925286766559 / 360.0)
        {
        }

        WavePattern::~WavePattern()
        {
        }

        std::string WavePattern::GetName() const
        {
            return bbox::Format("[Wave, period = %d ms, width = %f, gradient = %s]", m_period_ms, m_width, m_gradient->GetName());
        }
        
        void WavePattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time : %d ms\n", m_start_time_ms);
            out.Format("Period     : %d ms\n", m_period_ms);
            out.Format("Width      : %f\n", m_width);
            out.Format("Angle      : %f rad (%f deg)\n", m_angle, m_angle * 360.0 / 6.283185307179586476925286766559);
            out.Format("Gradient   : %s\n", m_gradient->GetName());
            m_gradient->PrintInformation(out);
        }

        std::vector<leds::Color> WavePattern::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            // Cartesian co-ordinates of the angle

            double cx = std::cos(m_angle);
            double cy = std::sin(m_angle);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                // Cartesian co-ordinates of the LED, referenced
                // from the center of the sign

                double lx = 2.0 * (render.layout.entries[i].x - 0.5);
                double ly = 2.0 * (render.layout.entries[i].y - 0.5);

                // Dot product of the LED position with the angle - 
                // i.e. the factor that the LED is in line with the angle

                double dot = cx*lx + cy*ly;

                // Make the wave, including updating it with the
                // distance along the angle
                
                uint64_t time_loop = (render.time_ms - m_start_time_ms) % m_period_ms;

                uint8_t grad_progress = uint8_t((time_loop * 255) / m_period_ms);

                grad_progress = uint64_t(grad_progress + 256 - (255.0 * dot / m_width)) % 255;

                result[i] = m_gradient->Convert(grad_progress / 256.0);
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
