/**
 * @file
 *
 * Implementation file for the ledsigns::common::FadeTransition class.
 */

#include <ledsigns/common/FadeTransition.h>

namespace ledsigns
{
    namespace common
    {

        FadeTransition::FadeTransition(const common::RenderState &render, uint64_t fade_time_ms)
            : m_start_time_ms(render.time_ms)
            , m_fade_time_ms(fade_time_ms)
            , m_complete(false)
        {
        }

        FadeTransition::~FadeTransition()
        {
        }

        std::string FadeTransition::GetName() const
        {
            return bbox::Format("Fade in %d ms", m_fade_time_ms);
        }
        
        void FadeTransition::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Start Time (ms): %d\n", m_start_time_ms);
            out.Format("Fade  Time (ms): %d\n", m_fade_time_ms);
        }

        std::vector<leds::Color> FadeTransition::Render(const common::RenderState &render)
        {
            uint8_t fade = 255;

            uint64_t delay_ms = render.time_ms - m_start_time_ms;

            if (delay_ms < m_fade_time_ms)
            {
                fade = uint8_t((delay_ms * 255) / m_fade_time_ms);
            }
            else
            {
                m_complete = true;
            }

            return std::vector<leds::Color>(
                render.layout.num_leds,
                leds::Color(fade, fade, fade));
        }

        bool FadeTransition::IsTransitionComplete() const
        {
            return m_complete;
        }

    } // namespace ledsigns::common
} // namespace ledsigns
