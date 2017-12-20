/**
* @file
*
* Header file for the ledsigns::casadelshade::FlickeringNeonPattern class.
*/

#ifndef __LEDSIGNS__CASADELSHADE__FLICKERING_NEON_PATTERN_H__
#define __LEDSIGNS__CASADELSHADE__FLICKERING_NEON_PATTERN_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace casadelshade
    {

        /**
         * Pattern that simulates a flickering neon sign
         * specific to the "Casa Del Shade" sign.
         */
        class FlickeringNeonPattern: public common::Pattern
        {
        public:

            explicit FlickeringNeonPattern(const common::RenderState &render);
            ~FlickeringNeonPattern();

            static common::Pattern::Factory Factory()
            {
                return [](const common::RenderState &render)
                {
                    return std::make_unique<FlickeringNeonPattern>(render);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            class Flicker
            {
                Flicker() = delete;
                Flicker(const Flicker &) = delete;
                Flicker &operator =(const Flicker &) = delete;

            public:
                explicit Flicker(const common::RenderState &render);

                bool On() const { return m_is_on; }

                std::string ToString() const;

                void Update(uint64_t time_ms);

            private:
                bool m_is_on;
                uint64_t m_trigger_time;
            };

            Flicker m_casa_flicker;
            Flicker m_e_flicker;
        };

    } // namespace ledsigns::casadelshade
} // namespace ledsigns

#endif // __LEDSIGNS__CASADELSHADE__FLICKERING_NEON_PATTERN_H__
