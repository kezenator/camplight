/**
 * @file
 *
 * Implementation file for the camplight::render::ColorGrid class.
 */

#include <camplight/render/ColorGrid.h>

namespace camplight
{
    namespace render
    {

        void ColorGrid::GetLedsForWebDisplay(std::vector<Color> &result, bool mask_center) const
        {
            result.resize(GRID_SIZE * GRID_SIZE);

            for (size_t i = 0; i < GRID_SIZE; i++)
            {
                for (size_t j = 0; j < GRID_SIZE; j++)
                {
                    size_t index = i + (GRID_SIZE * j);

                    result[index] = m_rows[i].leds[j];
                }
            }

            if (mask_center)
            {
                for (size_t i = ColorGrid::EDGE_WIDTH; i < (GRID_SIZE - ColorGrid::EDGE_WIDTH); i++)
                {
                    for (size_t j = ColorGrid::EDGE_WIDTH; j < (GRID_SIZE - ColorGrid::EDGE_WIDTH); j++)
                    {
                        size_t index = j + (GRID_SIZE * i);

                        result[index] = Color(0, 0, 0);
                    }
                }
            }
        }

    } // namespace camplight::render
} // namespace camplight
