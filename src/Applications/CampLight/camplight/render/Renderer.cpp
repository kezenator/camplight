/**
 * @file
 *
 * Implementation file for the camplight::render::Renderer class.
 */

#include <camplight/render/Renderer.h>

#ifndef WIN32
#include <ws2811.h>
#endif // !WIN32

// TODO - remove - just defaults for now
#include <camplight/pattern/DiagonalRainbow.h>
#include <camplight/pattern/Solid.h>
#include <camplight/pattern/Text.h>
#include <camplight/pattern/TestPattern.h>
#include <camplight/transition/LinearFade.h>
#include <camplight/transition/RandomDither.h>
#include <camplight/transition/Immediate.h>

namespace camplight
{
    namespace render
    {

#ifndef WIN32
        struct LedRun
        {
            int startx;
            int starty;
            int stepx;
            int stepy;
            int count;
        };

        void CopyLeds(const LedRun *led_runs, size_t run_length, const ColorGrid &grid, uint32_t *leds)
        {
            for (size_t run_counter = 0; run_counter < run_length; ++run_counter)
            {
                int x = led_runs->startx;
                int y = led_runs->starty;

                for (int led_counter = 0; led_counter < led_runs->count; ++led_counter)
                {
                    *leds = grid[x][y].ToWs2811Color();

                    x += led_runs->stepx;
                    y += led_runs->stepy;
                    ++leds;
                }

                ++led_runs;
            }
        }

        static LedRun main_led_runs[] = {
            { 0, 0, 1, 0, 9 },
            { 8, 1, -1, 0, 9 },
            { 0, 2, 1, 0, 9 },
            { 8, 3, -1, 0, 9 },
            { 0, 4, 1, 0, 9 },
            { 8, 5, -1, 0, 9 },
            { 0, 6, 1, 0, 9 },
            { 8, 7, -1, 0, 9 },
            { 0, 8, 1, 0, 9 },
        };

        static LedRun top_led_runs[] = {
            { 0, 0, 1, 0, 8 },
            { 8, 0, 0, 1, 8 },
            { 8, 8, -1, 0, 8 },
            { 0, 8, 0, -1, 8 },
            { 1, 1, 1, 0, 6 },
            { 7, 1, 0, 1, 6 },
            { 7, 7, -1, 0, 6 },
            { 1, 7, 0, -1, 6 },
        };
#endif // !WIN32

        struct Renderer::Hardware
        {
#ifndef WIN32
            ws2811_t ws2811;
#endif // !WIN32
        };

        Renderer::Renderer(const std::string &name, bbox::rt::Service &parent)
            : bbox::rt::Service(name, parent)
            , m_timer("timer", *this)
            , m_timestamp()
            , m_main_pattern_ptr()
            , m_top_pattern_ptr()
            , m_hardware_ptr()
            , m_main_rendered()
            , m_top_rendered()
        {
        }

        Renderer::~Renderer()
        {
        }

        void Renderer::HandleStarting()
        {
            m_timestamp = Timestamp();

            m_main_pattern_ptr = std::make_unique<pattern::TransitionPattern>();
            m_top_pattern_ptr = std::make_unique<pattern::TransitionPattern>();
            m_hardware_ptr = std::make_unique<Hardware>();

            m_main_pattern_ptr->ChangeTo(
                std::make_unique<pattern::TestPattern>(),
                std::make_unique<transition::Immediate>());
            m_top_pattern_ptr->ChangeTo(
                std::make_unique<pattern::TestPattern>(),
                std::make_unique<transition::Immediate>());

#ifndef WIN32
            ws2811_t &ws2811 = m_hardware_ptr->ws2811;

            ws2811.freq = WS2811_TARGET_FREQ;
            ws2811.dmanum = 5;

            ws2811.channel[0].gpionum = 18;
            ws2811.channel[0].invert = 0;
            ws2811.channel[0].count = (ColorGrid::GRID_SIZE * ColorGrid::GRID_SIZE) - ((ColorGrid::GRID_SIZE - 2 * ColorGrid::EDGE_WIDTH) * (ColorGrid::GRID_SIZE - 2 * ColorGrid::EDGE_WIDTH));
            ws2811.channel[0].brightness = 255;
            ws2811.channel[0].strip_type = WS2811_STRIP_GRB;

            ws2811.channel[1].gpionum = 19;
            ws2811.channel[1].invert = 0;
            ws2811.channel[1].count = ColorGrid::GRID_SIZE * ColorGrid::GRID_SIZE;
            ws2811.channel[1].brightness = 255;
            ws2811.channel[1].strip_type = WS2811_STRIP_GRB;

            if (ws2811_init(&m_hardware_ptr->ws2811))
            {
                std::cout << "Couldn't open WS2811 library" << std::endl;
                assert(false || "Couldn't open WS2811 library");
            }
#endif // !WIN32

            Render(m_timestamp, false);

            m_timer.SetHandler(std::bind(&Renderer::HandleTimeout, this));
            m_timer.StartPeriodic(bbox::rt::TimeSpan::Milliseconds(RENDER_PERIOD_MS));

            NotifyStarted();
        }

        void Renderer::HandleStopping()
        {
            m_timer.Cancel();

            m_main_pattern_ptr->ChangeToImmediate(std::make_unique<pattern::Solid>(render::Color(0, 0, 0)));
            m_top_pattern_ptr->ChangeToImmediate(std::make_unique<pattern::Solid>(render::Color(0, 0, 0)));

            Render(m_timestamp, true);

#ifndef WIN32
            ws2811_wait(&m_hardware_ptr->ws2811);
            ws2811_fini(&m_hardware_ptr->ws2811);
#endif // !WIN32

            m_main_pattern_ptr.reset();
            m_top_pattern_ptr.reset();
            m_hardware_ptr.reset();

            RequestStopAllChildren();
            NotifyStopped();
        }

        void Renderer::PrintState(bbox::DebugOutput &out) const
        {
            // TODO
            (void)out;
        }
        
        void Renderer::GetLedsForWebDisplay(uint32_t &width, uint32_t &height, std::vector<Color> &main_leds, std::vector<Color> &top_leds) const
        {
            width = ColorGrid::GRID_SIZE;
            height = ColorGrid::GRID_SIZE;

            m_main_rendered.GetLedsForWebDisplay(main_leds, false);
            m_top_rendered.GetLedsForWebDisplay(top_leds, true);
        }

        void Renderer::ChangeMainPattern(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<Transition> &&transition)
        {
            m_main_pattern_ptr->ChangeTo(std::move(pattern), std::move(transition));
        }

        void Renderer::ChangeTopPattern(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<Transition> &&transition)
        {
            m_top_pattern_ptr->ChangeTo(std::move(pattern), std::move(transition));
        }

        void Renderer::HandleTimeout()
        {
            m_timestamp = Timestamp(m_timestamp.ToMilliseconds() + RENDER_PERIOD_MS);

            Render(m_timestamp, true);
        }

        void Renderer::Render(Timestamp timestamp, bool wait_first)
        {
            if (wait_first)
            {
#ifndef WIN32
                ws2811_wait(&m_hardware_ptr->ws2811);
#endif // !WIN32
            }

            m_main_pattern_ptr->RenderTo(m_main_rendered, m_timestamp);
            m_top_pattern_ptr->RenderTo(m_top_rendered, m_timestamp);

#ifndef WIN32

            CopyLeds(main_led_runs, 9, m_main_rendered, m_hardware_ptr->ws2811.channel[1].leds);
            CopyLeds(top_led_runs, 8, m_top_rendered, m_hardware_ptr->ws2811.channel[0].leds);

            ws2811_render(&m_hardware_ptr->ws2811);

#endif // !WIN32
        }

    } // namespace camplight::render
} // namespace camplight
