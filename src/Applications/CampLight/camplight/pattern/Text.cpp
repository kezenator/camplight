/**
 * @file
 *
 * Implementation file for the camplight::pattern::Text class.
 */

#include <camplight/pattern/Text.h>
#include <bbox/Format.h>

extern const uint8_t *Jdpage_8x8;

namespace camplight
{
    namespace pattern
    {

        Text::Text(const std::string &text)
            : m_text(text)
        {
        }

        Text::~Text()
        {
        }

        std::string Text::GetDescription() const
        {
            return bbox::Format("Text[text=%s]", m_text);
        }

        void Text::RenderTo(render::ColorGrid &dest, render::Timestamp timestamp)
        {
            static constexpr size_t IMAGE_PIXELS_PER_ROW = 128;
            static constexpr size_t IMAGE_CHAR_PIXELS = 8;
            static constexpr size_t IMAGE_CHARS_PER_ROW = 16;

            // Work out the colors for each row

            render::Color row_colors[IMAGE_CHAR_PIXELS] =
            {
                render::Color(255, 0, 0),
                render::Color(255, 128, 0),
                render::Color(255, 255, 0),
                render::Color(0, 255, 0),
                render::Color(0, 255, 255),
                render::Color(0, 0, 255),
                render::Color(128, 0, 255),
                render::Color(255, 0, 128),
            };

            // Calculate the pixel offset

            size_t pixels_wide = IMAGE_CHAR_PIXELS * m_text.size();

            // Do the scrolling

            size_t pixel_offset = (timestamp.ToMilliseconds() / 125) % pixels_wide;

            // Paint the text

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                size_t pixel_index = (pixel_offset + i) % pixels_wide;
                size_t char_index = pixel_index / IMAGE_CHAR_PIXELS;
                size_t char_col = pixel_index % IMAGE_CHAR_PIXELS;
                size_t char_val = m_text[char_index];

                size_t image_x = (char_val % IMAGE_CHARS_PER_ROW) * IMAGE_CHAR_PIXELS;
                size_t image_y = (char_val / IMAGE_CHARS_PER_ROW) * IMAGE_CHAR_PIXELS;

                for (size_t j = 0; j < IMAGE_CHAR_PIXELS; ++j)
                {
                    // Now get the data from the RGB array.

                    size_t data_offset = 3 * ((image_x + char_col) + ((image_y + j) * IMAGE_PIXELS_PER_ROW));

                    render::Color color{
                        Jdpage_8x8[data_offset],
                        Jdpage_8x8[data_offset + 1],
                        Jdpage_8x8[data_offset + 2] };

                    if ((color.GetRed() == 255)
                        && (color.GetGreen() == 0)
                        && (color.GetBlue() == 255))
                    {
                        // Magenta - transparent - make black
                        color = render::Color();
                    }
                    else
                    {
                        color = row_colors[j];
                    }

                    dest[i][j] = color;
                }
            }

            // Finally, fill in the last row as black

            for (size_t i = 0; i < GRID_SIZE; ++i)
            {
                for (size_t j = IMAGE_CHAR_PIXELS; j < GRID_SIZE; ++j)
                {
                    dest[i][j] = render::Color();
                }
            }
        }

    } // namespace camplight::pattern
} // namespace camplight
