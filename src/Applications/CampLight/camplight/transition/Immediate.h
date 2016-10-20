/**
 * @file
 *
 * Header file for the camplight::transition::Immediate class.
 */

#ifndef __CAMPLIGHT__TRANSITION__IMMEDIATE_H__
#define __CAMPLIGHT__TRANSITION__IMMEDIATE_H__

#include <camplight/render/Transition.h>

namespace camplight
{
    namespace transition
    {

        /**
         * A linear fade between two patterns.
         */
        class Immediate : public render::Transition
        {
        public:

            Immediate();
            ~Immediate();

            std::string GetDescription() const override;
            Result RenderTransition(
                const render::ColorGrid &from,
                const render::ColorGrid &to,
                render::ColorGrid &dest,
                render::Timestamp timestamp) override;
        };

    } // namespace camplight::transition
} // namespace camplight

#endif // __CAMPLIGHT__TRANSITION__IMMEDIATE_H__
