/**
 * @file
 *
 * Implementation file for the camplight::pattern::DiagonalRainbox class.
 */

#include <camplight/pattern/DiagonalRainbow.h>

namespace camplight
{
    namespace pattern
    {

        DiagonalRainbox::DiagonalRainbox()
        {
        }

        DiagonalRainbox::~DiagonalRainbox()
        {
        }

        std::string DiagonalRainbox::GetDescription() const
        {
            return "Diagnoal-Rainbow";
        }

        void DiagonalRainbox::RenderTo(render::ColorGrid &dest, render::Timestamp timestamp)
        {
            static constexpr uint8_t TIME_RATE = 255 / 6;
            static constexpr uint8_t SPACE_RATE = 255 / GRID_SIZE;

            uint8_t overall_offset = uint8_t(timestamp.ToMilliseconds() * TIME_RATE / 1000);

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = 0; j < GRID_SIZE; ++j)
                {
                    uint8_t led_offset = uint8_t(overall_offset + (i * SPACE_RATE) + (j * SPACE_RATE));

                    dest[i][j] = render::HsvColor(led_offset, 255, 255).ToRgb();
                }
            }
        }

    } // namespace camplight::pattern
} // namespace camplight
