/**
 * @file
 *
 * Implementation file for the ledsigns::casadelshade::FlickeringNeonPattern class.
 */

#include <ledsigns/casadelshade/FlickeringNeonPattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        FlickeringNeonPattern::FlickeringNeonPattern(const common::RenderState &render)
            : m_casa_flicker(render)
            , m_e_flicker(render)
        {
        }

        FlickeringNeonPattern::~FlickeringNeonPattern()
        {
        }

        std::string FlickeringNeonPattern::GetName() const
        {
            return "Flickering Neon";
        }
        
        void FlickeringNeonPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Casa Flicker: %s\n", m_casa_flicker.ToString());
            out.Format("'E'  Flicker: %s\n", m_e_flicker.ToString());
        }

        std::vector<leds::Color> FlickeringNeonPattern::Render(const common::RenderState &render)
        {
            std::vector<leds::Color> result(render.layout.num_leds);

            m_casa_flicker.Update(render.time_ms);
            m_e_flicker.Update(render.time_ms);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                size_t symbol_num = render.layout.entries[i].symbol_num;

                if (((symbol_num == 0) && m_casa_flicker.On())
                    || (symbol_num == 1)
                    || (symbol_num == 2)
                    || ((symbol_num == 3) && m_e_flicker.On()))
                {
                    result[i] = leds::Color(0, 255, 255);
                }
            }

            return result;
        }

        FlickeringNeonPattern::Flicker::Flicker(const common::RenderState &render)
            : m_is_on(true)
            , m_trigger_time(render.time_ms + 2000 + (rand() % 3000))
        {
        }

        std::string FlickeringNeonPattern::Flicker::ToString() const
        {
            return bbox::Format("[is_on = %s, trigger_time = %d]", m_is_on, m_trigger_time);
        }

        void FlickeringNeonPattern::Flicker::Update(uint64_t time_ms)
        {
            if (time_ms >= m_trigger_time)
            {
                if (m_is_on)
                {
                    // Turn off, for a short period of time

                    m_is_on = false;

                    m_trigger_time = time_ms + 30 + (rand() % 50);
                }
                else
                {
                    // Turn on, for a short or long period

                    m_is_on = true;

                    if ((rand() % 100) < 20)
                        m_trigger_time = time_ms + 30 + (rand() % 50);
                    else
                        m_trigger_time = time_ms + 1000 + (rand() % 3000);
                }
            }
        }

    } // namespace ledsigns::casadelshade
} // namespace ledsigns
