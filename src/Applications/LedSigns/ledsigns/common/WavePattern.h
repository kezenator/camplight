/**
* @file
*
* Header file for the ledsigns::common::WavePattern class.
*/

#ifndef __LEDSIGNS__COMMON__WAVE_PATTERN_H__
#define __LEDSIGNS__COMMON__WAVE_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Gradient.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that generates a wave moving
         * across the board.
         */
        class WavePattern : public common::Pattern
        {
        public:

            WavePattern(const common::RenderState &render, uint64_t period_ms, double width, Gradient::Ptr gradient);
            ~WavePattern();

            static common::Pattern::Factory Factory(uint64_t period_ms, double width, Gradient::Ptr gradient)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<WavePattern>(render, period_ms, width, gradient);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const uint64_t m_start_time_ms;
            const uint64_t m_period_ms;
            const double m_width;
            const Gradient::Ptr m_gradient;
            const double m_angle;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__WAVE_PATTERN_H__
