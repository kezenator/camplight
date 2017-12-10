/**
* @file
*
* Header file for the ledsigns::casadelshade::StaggerPattern class.
*/

#ifndef __LEDSIGNS__CASADELSHADE__STAGGER_PATTERN_H__
#define __LEDSIGNS__CASADELSHADE__STAGGER_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        /**
         * Pattern that turns on the three segments in order that is
         * specific to the "Casa Del Shade" sign.
         */
        class StaggerPattern : public common::Pattern
        {
        public:

            explicit StaggerPattern(const common::RenderState &render);
            ~StaggerPattern();

            static common::Pattern::Factory Factory()
            {
                return [](const common::RenderState &render)
                {
                    return std::make_unique<StaggerPattern>(render);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;
        };

    } // namespace ledsigns::casadelshade
} // namespace ledsigns

#endif // __LEDSIGNS__CASADELSHADE__STAGGER_PATTERN_H__
