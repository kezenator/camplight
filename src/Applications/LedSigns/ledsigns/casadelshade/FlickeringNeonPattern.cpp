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

        FlickeringNeonPattern::FlickeringNeonPattern(const common::RenderState & /*render*/)
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
            out.Format("No state");
        }

        std::vector<leds::Color> FlickeringNeonPattern::Render(const common::RenderState &render)
        {
            std::vector<leds::Color> result(render.layout.num_leds);

            class Flicker
            {
            private:
                uint64_t off_time;
                uint64_t on_time;
                bool on;

            public:

                Flicker()
                    : off_time(0)
                    , on_time(0)
                    , on(false)
                {
                }

                void Update(uint64_t time_ms)
                {
                    if (time_ms >= on_time)
                    {
                        off_time = time_ms + 1000 + (rand() % 2500);
                        on_time = off_time + 50 + (rand() % 150);
                    }

                    on = (time_ms < off_time);
                }

                bool On() const
                {
                    return on;
                }
            };

            static Flicker casa;
            static Flicker e;

            casa.Update(render.time_ms);
            e.Update(render.time_ms);

            for (size_t i = 0; i < render.layout.num_leds; ++i)
            {
                size_t symbol_num = render.layout.entries[i].symbol_num;

                if (((symbol_num == 0) && casa.On())
                    || (symbol_num == 1)
                    || (symbol_num == 2)
                    || ((symbol_num == 3) && e.On()))
                {
                    result[i] = leds::Color(0, 255, 255);
                }
            }

            return result;
        }

    } // namespace ledsigns::casadelshade
} // namespace ledsigns
