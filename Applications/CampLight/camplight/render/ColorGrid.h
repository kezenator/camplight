/**
 * @file
 *
 * Header file for the camplight::render::ColorGrid class.
 */

#ifndef __CAMPLIGHT__RENDER__COLOR_GRID_H__
#define __CAMPLIGHT__RENDER__COLOR_GRID_H__

#include <camplight/render/Color.h>
#include <vector>

namespace camplight
{
    namespace render
    {

        /**
         * A grid of colors.
         */
        class ColorGrid
        {
            ColorGrid(const ColorGrid &) = delete;
            ColorGrid &operator =(const ColorGrid &) = delete;

            class Row;

        public:
            static constexpr size_t GRID_SIZE = 9;
            static constexpr size_t EDGE_WIDTH = 2;

            ColorGrid()
            {
            }

            ~ColorGrid()
            {
            }

            void GetLedsForWebDisplay(std::vector<Color> &result, bool mask_center) const;

            const Row &operator [](size_t index) const { return m_rows[index]; }
            Row &operator [](size_t index) { return m_rows[index]; }

        private:
            class Row
            {
                friend class ColorGrid;

            public:
                const Color &operator [](size_t index) const { return leds[index]; }
                Color &operator [](size_t index) { return leds[index]; }

            private:
                Color leds[GRID_SIZE];
            };

            Row m_rows[GRID_SIZE];
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__COLOR_GRID_H__
