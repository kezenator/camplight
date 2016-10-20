/**
 * @file
 *
 * Header file for the camplight::pattern::TestPattern class.
 */

#ifndef __CAMPLIGHT__PATTERN__TEST_PATTERN_H__
#define __CAMPLIGHT__PATTERN__TEST_PATTERN_H__

#include <camplight/render/Pattern.h>

namespace camplight
{
    namespace pattern
    {

        /**
         * A pattern that is a solid color.
         */
        class TestPattern : public render::Pattern
        {
        public:
            TestPattern();
            ~TestPattern();

            std::string GetDescription() const override;
            void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) override;
        };

    } // namespace camplight::pattern
} // namespace camplight

#endif // __CAMPLIGHT__PATTERN__TEST_PATTERN_H__
