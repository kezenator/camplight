/**
* @file
*
* Header file for the ledsigns::common::RenderState class.
*/

#ifndef __LEDSIGNS__COMMON__RENDER_STATE_H__
#define __LEDSIGNS__COMMON__RENDER_STATE_H__

#include <vector>
#include <leds/Color.h>        
#include <ledsigns/common/Layout.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * State about the render request that's
         * passed to the pattern when it's created
         * (for initialization), and also when rendered.
         */
        class RenderState
        {
            RenderState() = delete;
            RenderState(const RenderState &) = delete;
            RenderState &operator =(const RenderState &) = delete;

        public:

            ~RenderState() = default;

            RenderState(Layout &_layout, uint64_t _time_ms)
                : layout(_layout)
                , time_ms(_time_ms)
            {
            }

            Layout &layout;
            uint64_t time_ms;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__RENDER_STATE_H__
