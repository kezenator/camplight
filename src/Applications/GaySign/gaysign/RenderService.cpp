/**
 * @file
 *
 * Implementation file for the gaysign::RenderService class.
 *
 */

#include <gaysign/RenderService.h>
#include <gaysign/Layout.h>

namespace gaysign
{

    RenderService::RenderService(const std::string &name,
                                    bbox::rt::Service &parent,
                                    leds::FadecandyClient &fadecandy_client,
                                    leds::GpioClient &gpio_client)
        : bbox::rt::Service(name, parent)
        , m_fadecandy_client(fadecandy_client)
        , m_gpio_client(gpio_client)
        , m_frame_timer("frame-timer", *this)
    {
        m_frame_timer.SetHandler(std::bind(&RenderService::HandleFrameTimeout, this));

        SetThisDependantOn(fadecandy_client);
        SetThisDependantOn(gpio_client);
    }

    void RenderService::HandleStarting()
    {
        m_frame_timer.StartPeriodic(
            bbox::rt::TimeSpan::Milliseconds(30),
            bbox::rt::TimeSpan::Milliseconds(30));

        NotifyStarted();
    }

    void RenderService::HandleStopping()
    {
        m_frame_timer.Cancel();

        RequestStopAllChildren();
        NotifyStopped();
    }

    void RenderService::PrintState(bbox::DebugOutput &out) const
    {
        out.Format("TODO - render state\n");
    }

    void RenderService::HandleFrameTimeout()
    {
        m_gpio_client.SetOutput(BUTTON_OUTPUT, !m_gpio_client.GetInput(BUTTON_INPUT));

        using Pattern = std::function<std::vector<leds::Color>(uint64_t)>;

        static uint64_t time_ms = 0;

        time_ms += 30;

        Pattern rainbow_symbols = [](uint64_t time_ms) -> std::vector<leds::Color>
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

            std::vector<leds::Color> result(Layout::NUM_LEDS);

            constexpr uint64_t DELAY_TIME = 10 * 1000;
            constexpr uint64_t FADE_TIME = 2 * 1000;
            constexpr uint64_t COLOR_TIME = DELAY_TIME + 2 * FADE_TIME;
            const uint64_t PERIOD_TIME = colors.size() * COLOR_TIME;

            uint64_t mod_period = time_ms % PERIOD_TIME;
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

            for (size_t i = 0; i < Layout::NUM_LEDS; ++i)
            {
                if (Layout::ENTRIES[i].symbol)
                    result[i] = col1;
                else
                    result[i] = col2;
            }

            return result;
        };

        Pattern rainbow_wave = [](uint64_t time_ms) -> std::vector<leds::Color>
        {
            static constexpr uint64_t PERIOD = 10000;

            std::vector<leds::Color> result(Layout::NUM_LEDS);

            uint64_t mod_period = time_ms % PERIOD;

            uint8_t hue_base = uint8_t(mod_period * 255 / PERIOD);

            for (size_t i = 0; i < Layout::NUM_LEDS; ++i)
            {
                double pos = Layout::ENTRIES[i].x + Layout::ENTRIES[i].y;

                uint8_t hue_offset = uint8_t(64.0 * pos);

                result[i] = leds::HsvColor(hue_base + hue_offset, 255, 255).ToRgb();
            }

            return result;
        };
        
        std::vector<leds::Color> result = rainbow_symbols(time_ms);

        m_fadecandy_client.UpdateLeds(result);
    }

} // namespace camplight
