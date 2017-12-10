/**
* @file
*
* Header file for the ledsigns::common::FadeTransition class.
*/

#ifndef __LEDSIGNS__COMMON__FADE_MASK_H__
#define __LEDSIGNS__COMMON__FADE_MASK_H__

#include <ledsigns/common/Transition.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A transition that fades from black to white.
         */
        class FadeTransition : public common::Transition
        {
        public:

            explicit FadeTransition(const common::RenderState &render, uint64_t fade_time_ms);
            ~FadeTransition();

            static common::Transition::Factory Factory(uint64_t fade_time_ms)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<FadeTransition>(render, fade_time_ms);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

            bool IsTransitionComplete() const override;

        private:
            const uint64_t m_start_time_ms;
            const uint64_t m_fade_time_ms;
            bool m_complete;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__FADE_MASK_H__
