/**
 * @file
 *
 * Header file for the camplight::transition::Swipe class.
 */

#ifndef __CAMPLIGHT__TRANSITION__SWIPE_H__
#define __CAMPLIGHT__TRANSITION__SWIPE_H__

#include <camplight/render/Transition.h>

namespace camplight
{
    namespace transition
    {

        /**
         * A linear fade between two patterns.
         */
        class Swipe : public render::Transition
        {
            Swipe() = delete;

        public:

            explicit Swipe(bbox::rt::TimeSpan duration);
            ~Swipe();

            std::string GetDescription() const override;
            Result RenderTransition(
                const render::ColorGrid &from,
                const render::ColorGrid &to,
                render::ColorGrid &dest,
                render::Timestamp timestamp) override;

        private:
            const bbox::rt::TimeSpan m_duration;
            const size_t m_direction;
        };

    } // namespace camplight::transition
} // namespace camplight

#endif // __CAMPLIGHT__TRANSITION__SWIPE_H__
