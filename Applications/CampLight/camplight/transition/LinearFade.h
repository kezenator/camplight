/**
 * @file
 *
 * Header file for the camplight::transition::LinearFace class.
 */

#ifndef __CAMPLIGHT__TRANSITION__LINEAR_FADE_H__
#define __CAMPLIGHT__TRANSITION__LINEAR_FADE_H__

#include <camplight/render/Transition.h>

namespace camplight
{
    namespace transition
    {

        /**
         * A linear fade between two patterns.
         */
        class LinearFade: public render::Transition
        {
            LinearFade() = delete;

        public:

            explicit LinearFade(bbox::rt::TimeSpan duration);
            ~LinearFade();

            std::string GetDescription() const override;
            Result RenderTransition(
                const render::ColorGrid &from,
                const render::ColorGrid &to,
                render::ColorGrid &dest,
                render::Timestamp timestamp) override;

        private:
            const bbox::rt::TimeSpan m_duration;
        };

    } // namespace camplight::transition
} // namespace camplight

#endif // __CAMPLIGHT__TRANSITION__LINEAR_FADE_H__
