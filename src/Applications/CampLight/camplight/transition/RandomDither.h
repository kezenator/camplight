/**
 * @file
 *
 * Header file for the camplight::transition::RandomDither class.
 */

#ifndef __CAMPLIGHT__TRANSITION__RANDOM_DITHER_H__
#define __CAMPLIGHT__TRANSITION__RANDOM_DITHER_H__

#include <camplight/render/Transition.h>

namespace camplight
{
    namespace transition
    {

        /**
         * A linear fade between two patterns.
         */
        class RandomDither : public render::Transition
        {
            RandomDither() = delete;

        public:

            explicit RandomDither(bbox::rt::TimeSpan duration);
            ~RandomDither();

            std::string GetDescription() const override;
            Result RenderTransition(
                const render::ColorGrid &from,
                const render::ColorGrid &to,
                render::ColorGrid &dest,
                render::Timestamp timestamp) override;

        private:
            const bbox::rt::TimeSpan m_duration;

            size_t m_num_changed;
            bool m_leds_changed[GRID_SIZE][GRID_SIZE];
        };

    } // namespace camplight::transition
} // namespace camplight

#endif // __CAMPLIGHT__TRANSITION__RANDOM_DITHER_H__
