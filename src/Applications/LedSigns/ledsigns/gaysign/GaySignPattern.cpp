/**
 * @file
 *
 * Implementation file for the ledsigns::gaysign::GaySignPattern class.
 */

#include <ledsigns/gaysign/GaySignPattern.h>

namespace ledsigns
{
    namespace gaysign
    {

        GaySignPattern::GaySignPattern(leds::GpioClient &gpio_client, const common::RenderState & /*render*/)
            : m_gpio_client(gpio_client)
        {
        }

        GaySignPattern::~GaySignPattern()
        {
        }

        std::string GaySignPattern::GetName() const
        {
            return "Gay Sign";
        }
        
        void GaySignPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("No state");
        }

        std::vector<leds::Color> GaySignPattern::Render(const common::RenderState &render)
        {
            static constexpr size_t BUTTON_INPUT = 17;
            static constexpr size_t BUTTON_OUTPUT = 27;

            m_gpio_client.SetOutput(BUTTON_OUTPUT, !m_gpio_client.GetInput(BUTTON_INPUT));

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

            constexpr uint64_t DELAY_TIME = 10 * 1000;
            constexpr uint64_t FADE_TIME = 2 * 1000;
            constexpr uint64_t COLOR_TIME = DELAY_TIME + 2 * FADE_TIME;
            const uint64_t PERIOD_TIME = colors.size() * COLOR_TIME;

            uint64_t mod_period = render.time_ms % PERIOD_TIME;
            uint64_t color_index = mod_period / COLOR_TIME;
            uint64_t color_time = mod_period % COLOR_TIME;

            leds::Color prev = colors[color_index];
            leds::Color cur = colors[(color_index + 1) % colors.size()];
            leds::Color next = colors[(color_index + 2) % colors.size()];

            leds::Color col1;
            leds::Color col2;

            if (color_time < FADE_TIME)
            {
                uint8_t fade = uint8_t(color_time * 255 / FADE_TIME);

                col1 = prev.Scale(255 - fade) + cur.Scale(fade);
                col2 = cur;
            }
            else if (color_time < (2 * FADE_TIME))
            {
                uint8_t fade = uint8_t((color_time - FADE_TIME) * 255 / FADE_TIME);

                col1 = cur;
                col2 = cur.Scale(255 - fade) + next.Scale(fade);
            }
            else
            {
                // Solid on next colors
                col1 = cur;
                col2 = next;
            }

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                if (render.layout.entries[i].symbol_num == 0)
                    result[i] = col1;
                else
                    result[i] = col2;
            }

            return result;
        }

    } // namespace ledsigns::gaysign
} // namespace ledsigns
