/**
 * @file
 *
 * Header file for the camplight::render::Pattern class.
 */

#ifndef __CAMPLIGHT__RENDER__PATTERN_H__
#define __CAMPLIGHT__RENDER__PATTERN_H__

#include <camplight/render/Color.h>
#include <camplight/render/HsvColor.h>

#include <camplight/render/ColorGrid.h>
#include <camplight/render/Timestamp.h>

namespace camplight
{
    namespace render
    {

        /**
         * A base class for patterns of LEDs arranged in a grid pattern.
         */
        class Pattern
        {
            Pattern(const Pattern &) = delete;
            Pattern &operator =(const Pattern &) = delete;

        public:
            static constexpr size_t GRID_SIZE = ColorGrid::GRID_SIZE;

            Pattern();
            virtual ~Pattern();

            virtual std::string GetDescription() const = 0;
            virtual void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) = 0;
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__PATTERN_H__
