/**
* @file
*
* Implementation file for the ledsigns::casadelshade::PulsePattern class.
*/

#include <ledsigns/casadelshade/PulsePattern.h>
#include <ledsigns/common/VectorGradient.h>
#include <ledsigns/common/NoisePattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        PulsePattern::PulsePattern(const common::RenderState & render)
            : m_cycle_start_time(render.time_ms)
            , m_pulse_period(3000)
            , m_pulse_gradient()
            , m_noise_pattern()
        {
            m_pulse_gradient = std::make_shared<common::VectorGradient>(
                "Pulse",
                common::VectorGradient::Map({
                    { 0.0, leds::Color(64, 64, 64) },
                    { 0.1, leds::Color(255, 255, 255) },
                    { 0.2, leds::Color(64, 64, 64) },
                    { 0.3, leds::Color(255, 255, 255) },
                }));

            common::Gradient::Ptr noise_grad = std::make_shared<common::VectorGradient>(
                "Reds-and-Oranges",
                common::VectorGradient::Map({
                    { 0.0, leds::Color(64, 0, 0) },
                    { 0.3, leds::Color(255, 0, 0) },
                    { 0.8, leds::Color(255, 128, 0) },
                    { 1.0, leds::Color(200, 200, 200) },
            }));

            m_noise_pattern = common::NoisePattern::Factory(
                noise_grad,
                0.2,
                350.0)(render);
        }

        PulsePattern::~PulsePattern()
        {
        }

        std::string PulsePattern::GetName() const
        {
            return "Pulse";
        }

        void PulsePattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Cycle start time (ms): %d\n", m_cycle_start_time);
            out.Format("Pulse period (ms):     %d\n", m_pulse_period);
            out.Format("Pulse Gradient:        %s\n", m_pulse_gradient->GetName());
            m_pulse_gradient->PrintInformation(out);
            out.Format("\n");
            out.Format("Noise Pattern: %s\n", m_noise_pattern->GetName());
            m_noise_pattern->PrintInformation(out);
        }

        std::vector<leds::Color> PulsePattern::Render(const common::RenderState &render)
        {
            double pulse_progress = double((render.time_ms - m_cycle_start_time) % m_pulse_period) / double(m_pulse_period);

            leds::Color pulse = m_pulse_gradient->Convert(pulse_progress);

            std::vector<leds::Color> result = m_noise_pattern->Render(render);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                result[i] = result[i].Scale(pulse);
            }

            return result;
        }

    } // namespace ledsigns::casadelshade
} // namespace ledsigns
