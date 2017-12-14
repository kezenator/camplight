/**
 * @file
 *
 * Implementation file for the ledsigns::common::SwipeTransition class.
 */

#include <ledsigns/common/SwipeTransition.h>

#include <cmath>

namespace ledsigns
{
    namespace common
    {

        SwipeTransition::SwipeTransition(const common::RenderState &render, uint64_t swipe_time_ms)
            : m_start_time_ms(render.time_ms)
            , m_swipe_time_ms(swipe_time_ms)
            , m_angle((rand() % 360) * 6.283185307179586476925286766559 / 360.0)
            , m_complete(false)
        {
        }

        SwipeTransition::~SwipeTransition()
        {
        }

        std::string SwipeTransition::GetName() const
        {
            return bbox::Format("Swipe in %d ms", m_swipe_time_ms);
        }

        void SwipeTransition::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time (ms): %d\n", m_start_time_ms);
            out.Format("Swipe Time (ms): %d\n", m_swipe_time_ms);
            out.Format("Angle          : %f\n", m_angle * 360.0 / 6.283185307179586476925286766559);
        }

        std::vector<leds::Color> SwipeTransition::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            // Cartesian co-ordinates of the angle

            double cx = std::cos(m_angle);
            double cy = std::sin(m_angle);

            // The distance threshold (from -1 to +1) at which
            // the fade is currently occurring

            double threshold = 1.0;

            if ((render.time_ms - m_start_time_ms) < m_swipe_time_ms)
            {
                threshold = ((double(render.time_ms - m_start_time_ms) / m_swipe_time_ms) * 2.0) - 1.0;
            }
            else
            {
                m_complete = true;
            }

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                // Cartesian co-ordinates of the LED, referenced
                // from the center of the sign

                double lx = 2.0 * (render.layout.entries[i].x - 0.5);
                double ly = 2.0 * (render.layout.entries[i].y - 0.5);

                // Dot product of the LED position with the angle - 
                // i.e. the factor that the LED is in line with the angle

                double dot = cx*lx + cy*ly;

                // Now turn it on if we've 'reached' the threshold

                if (dot < threshold)
                    result[i] = leds::Color(255, 255, 255);
            }

            return result;
        }

        bool SwipeTransition::IsTransitionComplete() const
        {
            return m_complete;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
