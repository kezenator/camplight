/**
* @file
*
* Implementation file for the ledsigns::casadelshade::PulsePattern class.
*/

#include <ledsigns/casadelshade/PulsePattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        PulsePattern::PulsePattern(const common::RenderState & render)
            : m_cycle_start_time(render.time_ms)
        {
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
        }

        std::vector<leds::Color> PulsePattern::Render(const common::RenderState &render)
        {
            static const uint64_t PERIOD = 5000;
            static const uint64_t SECOND_ON_TIME = 1500;

            uint64_t delay = render.time_ms - m_cycle_start_time;
            while (delay >= PERIOD)
            {
                m_cycle_start_time += PERIOD;
                delay = render.time_ms - m_cycle_start_time;
            }

            uint64_t progress, limit;

            if (delay < SECOND_ON_TIME)
            {
                limit = SECOND_ON_TIME / 3;

                if (delay < (SECOND_ON_TIME / 3))
                    progress = delay;
                else if (delay < ((2 * SECOND_ON_TIME) / 3))
                    progress = (SECOND_ON_TIME / 3) - (delay - (SECOND_ON_TIME / 3));
                else
                    progress = delay - ((2 * SECOND_ON_TIME) / 3);
            }
            else
            {
                progress = (PERIOD - SECOND_ON_TIME) - (delay - SECOND_ON_TIME);
                limit = PERIOD - SECOND_ON_TIME;
            }

            uint8_t fade = uint8_t(255 * progress / limit);

            return std::vector<leds::Color>(
                render.layout.num_leds,
                leds::Color(fade, 0, 0));
        }

    } // namespace ledsigns::casadelshade
} // namespace ledsigns
