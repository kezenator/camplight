/**
* @file
*
* Header file for the ledsigns::common::StaggerPattern class.
*/

#ifndef __LEDSIGNS__COMMON__STAGGER_PATTERN_H__
#define __LEDSIGNS__COMMON__STAGGER_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * Pattern that turns on segments of a sign on in order.
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

        private:
            size_t m_num_segments;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__STAGGER_PATTERN_H__
