/**
* @file
*
* Header file for the ledsigns::common::Transition class.
*/

#ifndef __LEDSIGNS__COMMON__TRANSITION_H__
#define __LEDSIGNS__COMMON__TRANSITION_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that changes (in some way) from black
         * to white, to provide a transition between patterns.
         * It must indicate when it's complete.
         */
        class Transition: public Pattern
        {
        public:

            using Ptr = std::unique_ptr<Transition>;
            using Factory = std::function<Ptr(const RenderState &)>;

            virtual bool IsTransitionComplete() const = 0;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__TRANSITION_H__
