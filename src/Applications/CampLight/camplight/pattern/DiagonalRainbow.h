/**
 * @file
 *
 * Header file for the camplight::pattern::DiagonalRainbox class.
 */

#ifndef __CAMPLIGHT__PATTERN__DIAGONAL_RAINBOW_H__
#define __CAMPLIGHT__PATTERN__DIAGONAL_RAINBOW_H__

#include <camplight/render/Pattern.h>

namespace camplight
{
    namespace pattern
    {

        /**
         * A basic diagonal scrolling rainbow pattern.
         */
        class DiagonalRainbox: public render::Pattern
        {
        public:
            DiagonalRainbox();
            ~DiagonalRainbox();

            std::string GetDescription() const override;
            void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) override;
        };

    } // namespace camplight::pattern
} // namespace camplight

#endif // __CAMPLIGHT__PATTERN__DIAGONAL_RAINBOW_H__
