/**
 * @file
 *
 * Header for the bbox::rt::WallClock class.
 */

#ifndef __BBOX__RT__WALL_CLOCK_H__
#define __BBOX__RT__WALL_CLOCK_H__

#pragma once

#include <string>

namespace bbox {
    namespace rt {

        /**
         * Helper class.
         */
        class WallClock
        {
            // Can't construct
            WallClock() = delete;
            WallClock(const WallClock &) = delete;
            void operator =(const WallClock &) = delete;

        public:

            static std::string GetCurrentTimeRfc1123();
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__WALL_CLOCK_H__