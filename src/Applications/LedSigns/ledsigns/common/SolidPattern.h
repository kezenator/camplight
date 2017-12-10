/**
* @file
*
* Header file for the ledsigns::common::SolidPattern class.
*/

#ifndef __LEDSIGNS__COMMON__SOLID_PATTERN_H__
#define __LEDSIGNS__COMMON__SOLID_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that generates a solid color.
         */
        class SolidPattern : public common::Pattern
        {
        public:

            explicit SolidPattern(const common::RenderState &render, leds::Color color);
            ~SolidPattern();

            static common::Pattern::Factory Factory(leds::Color color)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<SolidPattern>(render, color);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            leds::Color m_color;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__SOLID_PATTERN_H__
