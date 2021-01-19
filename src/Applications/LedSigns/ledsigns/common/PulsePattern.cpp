/**
* @file
*
* Implementation file for the ledsigns::common::PulsePattern class.
*/

#include <ledsigns/common/PulsePattern.h>
#include <ledsigns/common/VectorGradient.h>
#include <ledsigns/common/NoisePattern.h>

namespace ledsigns
{
    namespace common
    {

        PulsePattern::PulsePattern(const common::RenderState & render, common::Pattern::Ptr&& color_pattern)
            : m_cycle_start_time(render.time_ms)
            , m_pulse_period(3000)
            , m_pulse_gradient()
            , m_color_pattern(std::move(color_pattern))
        {
            m_pulse_gradient = std::make_shared<common::VectorGradient>(
                "Pulse",
                common::VectorGradient::Map({
                    { 0.0, leds::Color(64, 64, 64) },
                    { 0.1, leds::Color(255, 255, 255) },
                    { 0.2, leds::Color(64, 64, 64) },
                    { 0.3, leds::Color(255, 255, 255) },
                }));
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
            out.Format("Color Pattern: %s\n", m_color_pattern->GetName());
            m_color_pattern->PrintInformation(out);
        }

        std::vector<leds::Color> PulsePattern::Render(const common::RenderState &render)
        {
            double pulse_progress = double((render.time_ms - m_cycle_start_time) % m_pulse_period) / double(m_pulse_period);

            leds::Color pulse = m_pulse_gradient->Convert(pulse_progress);

            std::vector<leds::Color> result = m_color_pattern->Render(render);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                result[i] = result[i].Scale(pulse);
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
