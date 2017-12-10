/**
* @file
*
* Header file for the ledsigns::common::Mask class.
*/

#ifndef __LEDSIGNS__COMMON__MASK_H__
#define __LEDSIGNS__COMMON__MASK_H__

#include <ledsigns/common/Pattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that provides a mas
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

#endif // __LEDSIGNS__COMMON__MASK_H__
