/**
 * @file
 *
 * Implementation file for the ledsigns::common::SwipeTestPattern class.
 */

#include <ledsigns/common/SwipeTestPattern.h>

namespace ledsigns
{
    namespace common
    {

        SwipeTestPattern::SwipeTestPattern(const common::RenderState & /*render*/)
        {
        }

        SwipeTestPattern::~SwipeTestPattern()
        {
        }

        std::string SwipeTestPattern::GetName() const
        {
            return bbox::Format("Swipe Test Pattern");
        }
        
        void SwipeTestPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("No state");
        }

        std::vector<leds::Color> SwipeTestPattern::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            size_t mode = (render.time_ms / 3000) % 3;
            size_t progress = ((render.time_ms % 3000) * 511) / 3000;
            uint8_t fade;
            if (progress <= 255)
                fade = uint8_t(progress);
            else
                fade = uint8_t(511 - progress);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                double ix = render.layout.entries[i].x;
                double iy = render.layout.entries[i].y;

                double dist;

                if (mode == 0)
                    dist = ix;
                else if (mode == 1)
                    dist = iy;
                else
                    dist = (ix + iy) / 2.0;

                if ((fade / 256.0) > dist)
                    result[i] = leds::Color(255, 255, 255);
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
