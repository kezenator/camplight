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

        NoisePattern::NoisePattern(const common::RenderState &render, Gradient::Ptr &&gradient)
            : m_start_time(render.time_ms)
            , m_gradient(std::move(gradient))
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
            out.Format("Start Time (ms): %d\n", m_start_time);
            out.Format("Gradient:        %s\n", m_gradient->GetName());
            m_gradient->PrintInformation(out);
        }

        std::vector<leds::Color> NoisePattern::Render(const common::RenderState &render)
        {
            constexpr float SIZE = 250.0f;
            constexpr float FREQUENCY = 30.0f;

            std::vector<leds::Color> result(render.layout.num_leds);

            float time = (render.time_ms - m_start_time) / FREQUENCY;

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                float noise_value = m_noise.GetNoise(
                    SIZE * float(render.layout.entries[i].x),
                    SIZE * float(render.layout.entries[i].y),
                    time);

                if (noise_value > 1.0f)
                    noise_value = 1.0f;
                else if (noise_value < -1.0f)
                    noise_value = -1.0f;

                result[i] = m_gradient->Convert(0.5 + (0.5 * noise_value));
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
