/**
* @file
*
* Header file for the ledsigns::common::SwipeTestPattern class.
*/

#ifndef __LEDSIGNS__COMMON__SWIPE_TEST_PATTERN_H__
#define __LEDSIGNS__COMMON__SWIPE_TEST_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that displays swiping patterns
         * to test the x and y co-ordinates of the LEDs.
         */
        class SwipeTestPattern : public common::Pattern
        {
        public:

            explicit SwipeTestPattern(const common::RenderState &render);
            ~SwipeTestPattern();

            static common::Pattern::Factory Factory()
            {
                return [](const common::RenderState &render)
                {
                    return std::make_unique<SwipeTestPattern>(render);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__SWIPE_TEST_PATTERN_H__
