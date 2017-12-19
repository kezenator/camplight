/**
* @file
*
* Header file for the ledsigns::common::GrowingCirclesPattern class.
*/

#ifndef __LEDSIGNS__COMMON__GROWING_CIRCLES_PATTERN_H__
#define __LEDSIGNS__COMMON__GROWING_CIRCLES_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Gradient.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that generates circles
         * expanding from the center of the board.
         */
        class GrowingCirclesPattern : public common::Pattern
        {
        public:

            GrowingCirclesPattern(const common::RenderState &render, uint64_t period_ms, double width, Gradient::Ptr gradient);
            ~GrowingCirclesPattern();

            static common::Pattern::Factory Factory(uint64_t period_ms, double width, Gradient::Ptr gradient)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<GrowingCirclesPattern>(render, period_ms, width, gradient);
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
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__GROWING_CIRCLES_PATTERN_H__
