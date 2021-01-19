/**
 * @file
 *
 * Implementation file for the ledsigns::common::StaggerPattern class.
 */

#include <ledsigns/common/StaggerPattern.h>
#include <ledsigns/common/ColorSets.h>

namespace ledsigns
{
    namespace common
    {

        StaggerPattern::StaggerPattern(const common::RenderState &render)
            : m_num_segments(0)
        {
            for (const auto &entry : render.layout.entries)
            {
                if (entry.symbol_num >= m_num_segments)
                {
                    m_num_segments = entry.symbol_num + 1;
                }
            }
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
            out.Format("Num Segments: %d", m_num_segments);
        }

        std::vector<leds::Color> StaggerPattern::Render(const common::RenderState &render)
        {
            static std::vector<leds::Color> COLORS = ColorSets::FullRainbow();
            static size_t NUM_COLORS = COLORS.size();

            std::vector<leds::Color> result(render.layout.num_leds);

            constexpr size_t DURATION = 500;

            size_t total_duration = (m_num_segments + 1) * DURATION;

            size_t base_index = (render.time_ms / total_duration) % NUM_COLORS;
            size_t time_offset = render.time_ms % total_duration;

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                size_t symbol_num = render.layout.entries[i].symbol_num;

                if (time_offset > ((symbol_num + 1) * DURATION))
                {
                    result[i] = COLORS[(base_index + symbol_num) % NUM_COLORS];
                }
            }

            return result;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
