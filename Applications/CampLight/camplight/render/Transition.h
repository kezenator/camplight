/**
 * @file
 *
 * Header file for the camplight::render::Transition class.
 */

#ifndef __CAMPLIGHT__RENDER__TRANSITION_H__
#define __CAMPLIGHT__RENDER__TRANSITION_H__

#include <camplight/render/ColorGrid.h>
#include <camplight/render/Timestamp.h>

namespace camplight
{
    namespace render
    {

        /**
         * A base class for transitions between patterns.
         */
        class Transition
        {
            Transition(const Transition &) = delete;
            Transition &operator =(const Transition &) = delete;

        public:

            enum Result{
                TransitionProgressing,
                TransitionComplete,
            };

            static constexpr size_t GRID_SIZE = ColorGrid::GRID_SIZE;

            Transition();
            virtual ~Transition();

            virtual std::string GetDescription() const = 0; 
            virtual Result RenderTransition(
                const render::ColorGrid &from,
                const render::ColorGrid &to,
                render::ColorGrid &dest,
                render::Timestamp timestamp) = 0;
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__TRANSITION_H__
