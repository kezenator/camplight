/**
* @file
*
* Header file for the ledsigns::common::CountPattern class.
*/

#ifndef __LEDSIGNS__COMMON__COUNT_PATTERN_H__
#define __LEDSIGNS__COMMON__COUNT_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that identifies LEDs in order and
         * displays a pattern based on their x-y co-ordinates.
         */
        class CountPattern : public common::Pattern
        {
        public:

            explicit CountPattern(const common::RenderState &render);
            ~CountPattern();

            static common::Pattern::Factory Factory()
            {
                return [](const common::RenderState &render)
                {
                    return std::make_unique<CountPattern>(render);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__COUNT_PATTERN_H__
