/**
* @file
*
* Header file for the ledsigns::casadelshade::PulsePattern class.
*/

#ifndef __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__
#define __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__

#include <ledsigns/common/Pattern.h>

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
            uint64_t m_cycle_start_time;
        };

    } // namespace ledsigns::casadelshade
} // namespace ledsigns

#endif // __LEDSIGNS__CASADELSHADE__PULSE_PATTERN_H__
