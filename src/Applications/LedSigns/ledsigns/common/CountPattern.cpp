/**
 * @file
 *
 * Implementation file for the ledsigns::common::CountPattern class.
 */

#include <ledsigns/common/CountPattern.h>

namespace ledsigns
{
    namespace common
    {

        CountPattern::CountPattern(const common::RenderState & /*render*/)
        {
        }

        CountPattern::~CountPattern()
        {
        }

        std::string CountPattern::GetName() const
        {
            return bbox::Format("Count Pattern");
        }
        
        void CountPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("No state");
        }

        std::vector<leds::Color> CountPattern::Render(const common::RenderState &render)
        {
            auto result = std::vector<leds::Color>(render.layout.num_leds);

            size_t index = (render.time_ms / 3000) % render.layout.num_leds;
            double index_x = render.layout.entries[index].x;
            double index_y = render.layout.entries[index].y;

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                double ix = render.layout.entries[i].x;
                double iy = render.layout.entries[i].y;

                if (i == index)
                    result[i] = leds::Color(255, 255, 255);
                else if (ix == index_x)
                    result[i] = leds::Color(255, 0, 0);
                else if (iy == index_y)
                    result[i] = leds::Color(0, 255, 0);
                else if ((ix < index_x) && (iy < index_y))
                    result[i] = leds::Color(255, 255, 0);
                else if ((ix > index_x) && (iy > index_y))
                    result[i] = leds::Color(0, 0, 255);
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
