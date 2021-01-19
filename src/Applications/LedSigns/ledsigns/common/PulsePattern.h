/**
* @file
*
* Header file for the ledsigns::common::PulsePattern class.
*/

#ifndef __LEDSIGNS__COMMON__PULSE_PATTERN_H__
#define __LEDSIGNS__COMMON__PULSE_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Gradient.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * Pattern that pulses on and off.
         */
        class PulsePattern : public common::Pattern
        {
        public:

            explicit PulsePattern(const common::RenderState &render, common::Pattern::Ptr &&color_pattern);
            ~PulsePattern();

            static common::Pattern::Factory Factory(Pattern::Factory &&color_factory)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<PulsePattern>(render, color_factory(render));
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const uint64_t m_cycle_start_time;
            const uint64_t m_pulse_period;
            common::Gradient::Ptr m_pulse_gradient;
            common::Pattern::Ptr m_color_pattern;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__PULSE_PATTERN_H__
