/**
 * @file
 *
 * Implementation file for the ledsigns::common::NoisePattern class.
 */

#include <ledsigns/common/NoisePattern.h>

namespace ledsigns
{
    namespace common
    {

        NoisePattern::NoisePattern(const common::RenderState &render,
                                   Gradient::Ptr &&gradient,
                                   double patch_size,
                                   double change_rate_ms)
            : m_start_time(render.time_ms)
            , m_gradient(std::move(gradient))
            , m_patch_size(patch_size)
            , m_change_rate_ms(change_rate_ms)
            , m_noise()
        {
        }

        NoisePattern::~NoisePattern()
        {
        }

        std::string NoisePattern::GetName() const
        {
            return "Noise";
        }
        
        void NoisePattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time (ms):  %d\n", m_start_time);
            out.Format("Patch Size:       %.3\n", m_patch_size);
            out.Format("Change Rate (ms): %.1\n", m_change_rate_ms);
            out.Format("Gradient:         %s\n", m_gradient->GetName());
            m_gradient->PrintInformation(out);
        }

        std::vector<leds::Color> NoisePattern::Render(const common::RenderState &render)
        {
            std::vector<leds::Color> result(render.layout.num_leds);

            double time = (render.time_ms - m_start_time) / 50.0 / (m_change_rate_ms / 1000.0);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                double noise_value = m_noise.GetNoise(
                    25.0 * render.layout.entries[i].x / m_patch_size,
                    25.0 * render.layout.entries[i].y / m_patch_size,
                    time);

                if (noise_value > 1.0)
                    noise_value = 1.0;
                else if (noise_value < -1.0)
                    noise_value = -1.0;

                result[i] = m_gradient->Convert(0.5 + (0.5 * noise_value));
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
