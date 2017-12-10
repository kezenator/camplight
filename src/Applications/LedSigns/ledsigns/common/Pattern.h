/**
* @file
*
* Header file for the ledsigns::common::Pattern class.
*/

#ifndef __LEDSIGNS__COMMON__PATTERN_H__
#define __LEDSIGNS__COMMON__PATTERN_H__

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <bbox/DebugOutput.h>
#include <leds/Color.h>
#include <ledsigns/common/RenderState.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * Base class for patterns that can be rendered
         * to the LEDs in the box.
         */
        class Pattern
        {
            Pattern(const Pattern &) = delete;
            Pattern &operator =(const Pattern &) = delete;

        public:

            using Ptr = std::unique_ptr<Pattern>;
            using Factory = std::function<Ptr(const RenderState &)>;

            Pattern();
            virtual ~Pattern();

            virtual std::string GetName() const = 0;
            virtual void PrintInformation(bbox::DebugOutput &out) const = 0;

            virtual std::vector<leds::Color> Render(const RenderState &render) = 0;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__PATTERN_H__
