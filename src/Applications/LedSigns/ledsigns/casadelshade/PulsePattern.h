/**
* @file
*
* Header file for the ledsigns::casadelshade::PulsePattern class.
*/

#ifndef __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__
#define __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Gradient.h>

namespace ledsigns
{
    namespace casadelshade
    {

        /**
         * Pattern that pulses on and off.
         */
        class PulsePattern : public common::Pattern
        {
        public:

            explicit PulsePattern(const common::RenderState &render);
            ~PulsePattern();

            static common::Pattern::Factory Factory()
            {
                return [](const common::RenderState &render)
                {
                    return std::make_unique<PulsePattern>(render);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const uint64_t m_cycle_start_time;
            const uint64_t m_pulse_period;
            common::Gradient::Ptr m_pulse_gradient;
            common::Pattern::Ptr m_noise_pattern;
        };

    } // namespace ledsigns::casadelshade
} // namespace ledsigns

#endif // __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__
