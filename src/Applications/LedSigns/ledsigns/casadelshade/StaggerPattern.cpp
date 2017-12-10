/**
 * @file
 *
 * Implementation file for the ledsigns::casadelshade::StaggerPattern class.
 */

#include <ledsigns/casadelshade/StaggerPattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        StaggerPattern::StaggerPattern(const common::RenderState & /*render*/)
        {
        }

        StaggerPattern::~StaggerPattern()
        {
        }

        std::string StaggerPattern::GetName() const
        {
            return "Stagger";
        }
        
        void StaggerPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("No state");
        }

        std::vector<leds::Color> StaggerPattern::Render(const common::RenderState &render)
        {
            static std::vector<leds::Color> colors = {
                leds::Color(255, 0, 0),
                leds::Color(255, 128, 0),
                leds::Color(255, 255, 0),
                leds::Color(0, 255, 0),
                leds::Color(0, 255, 255),
                leds::Color(0, 0, 255),
                leds::Color(255, 0, 255),
            };

            std::vector<leds::Color> result(render.layout.num_leds);

            constexpr size_t DURATION = 2000;

            size_t base_index = (render.time_ms / DURATION) % 7;
            size_t time_offset = render.time_ms % DURATION;

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                size_t symbol_num = render.layout.entries[i].symbol_num;
                if (symbol_num > 2)
                    symbol_num = 2;

                if (((symbol_num == 0) && (time_offset > 500))
                    || ((symbol_num == 1) && (time_offset > 1000))
                    || ((symbol_num == 2) && (time_offset > 1500)))
                {
                    result[i] = colors[(base_index + symbol_num) % 7];
                }
            }

            return result;
        }

    } // namespace ledsigns::casadelshade
} // namespace ledsigns
