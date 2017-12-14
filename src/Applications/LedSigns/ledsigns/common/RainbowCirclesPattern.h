/**
* @file
*
* Header file for the ledsigns::common::RainbowCirclesPattern class.
*/

#ifndef __LEDSIGNS__COMMON__RAINBOW_CIRCLES_PATTERN_H__
#define __LEDSIGNS__COMMON__RAINBOW_CIRCLES_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that generates rainbow colour circles
         * expanding from the center of the board.
         */
        class RainbowCirclesPattern : public common::Pattern
        {
        public:

            explicit RainbowCirclesPattern(const common::RenderState &render, uint64_t period_ms, double width);
            ~RainbowCirclesPattern();

            static common::Pattern::Factory Factory(uint64_t period_ms, double width)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<RainbowCirclesPattern>(render, period_ms, width);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const uint64_t m_start_time_ms;
            const uint64_t m_period_ms;
            const double m_width;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__RAINBOW_CIRCLES_PATTERN_H__
