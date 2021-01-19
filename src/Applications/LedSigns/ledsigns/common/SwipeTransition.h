/**
 * @file
 *
 * Header file for the ledsigns::common::SwipeTransition class.
 */

#ifndef __LEDSIGNS__COMMON__SWIPE_TRANSITION_H__
#define __LEDSIGNS__COMMON__SWIPE_TRANSITION_H__

#include <ledsigns/common/Transition.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A transition that fades from black to white.
         */
        class SwipeTransition : public common::Transition
        {
        public:

            explicit SwipeTransition(const common::RenderState &render, uint64_t swipe_time_ms);
            ~SwipeTransition();

            static common::Transition::Factory RandomDirectionFactory(uint64_t swipe_time_ms)
            {
                return [=](const common::RenderState &render)
                {
                    return std::make_unique<SwipeTransition>(render, swipe_time_ms);
                };
            }

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

            bool IsTransitionComplete() const override;

        private:
            const uint64_t m_start_time_ms;
            const uint64_t m_swipe_time_ms;
            const double m_angle;
            bool m_complete;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__SWIPE_TRANSITION_H__
