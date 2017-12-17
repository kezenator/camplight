/**
 * @file
 *
 * Header file for the ledsigns::common::Gradient class.
 */

#ifndef __LEDSIGNS__COMMON__GRADIENT_H__
#define __LEDSIGNS__COMMON__GRADIENT_H__

#include <memory>
#include <string>
#include <bbox/DebugOutput.h>
#include <leds/Color.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * Base class for the color gradient class which
         * converts a scalar number [0..1] into a color.
         *
         * It also provides a number of pre-defined gradients.
         */
        class Gradient
        {
            Gradient(const Gradient &) = delete;
            Gradient &operator =(const Gradient &) = delete;

        public:

            using Ptr = std::shared_ptr<Gradient>;

            static Ptr FullRainbow();
            static Ptr GreensAndBlues();
            static Ptr RedsAndPurples();

            Gradient();
            virtual ~Gradient();

            virtual std::string GetName() const = 0;
            virtual void PrintInformation(bbox::DebugOutput &out) const = 0;

            virtual leds::Color Convert(double progress) const = 0;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__GRADIENT_H__
