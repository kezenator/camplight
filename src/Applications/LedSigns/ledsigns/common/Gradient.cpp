/**
 * @file
 *
 * Implementation file for the ledsigns::common::Graident class.
 */

#include <ledsigns/common/Gradient.h>
#include <ledsigns/common/VectorGradient.h>

namespace ledsigns
{
    namespace common
    {

        Gradient::Gradient()
        {
        }

        Gradient::~Gradient()
        {
        }

        Gradient::Ptr Gradient::FullRainbow()
        {
            return std::make_unique<VectorGradient>(
                "Full-Rainbow",
                VectorGradient::Map({
                    { 0.0 / 8.0, leds::Color(255, 0, 0) },
                    { 1.0 / 8.0, leds::Color(255, 128, 0) },
                    { 2.0 / 8.0, leds::Color(255, 255, 0) },
                    { 3.0 / 8.0, leds::Color(0, 255, 0) },
                    { 4.0 / 8.0, leds::Color(0, 255, 255) },
                    { 5.0 / 8.0, leds::Color(0, 0, 255) },
                    { 6.0 / 8.0, leds::Color(128, 0, 255) },
                    { 7.0 / 8.0, leds::Color(255, 0, 255) },
            }));
        }

        Gradient::Ptr Gradient::GreensAndBlues()
        {
            return std::make_unique<VectorGradient>(
                "Greens-and-Blues",
                VectorGradient::Map({
                    { 0.0 / 4.0, leds::Color(0, 255, 0) },
                    { 1.0 / 4.0, leds::Color(0, 255, 255) },
                    { 2.0 / 4.0, leds::Color(0, 0, 255) },
                    { 3.0 / 4.0, leds::Color(128, 0, 255) },
            }));
        }

        Gradient::Ptr Gradient::RedsAndPurples()
        {
            return std::make_unique<VectorGradient>(
                "Reds-and-Purples",
                VectorGradient::Map({
                    { 0.0 / 5.0, leds::Color(0, 255, 255) },
                    { 1.0 / 5.0, leds::Color(0, 0, 255) },
                    { 2.0 / 5.0, leds::Color(128, 0, 255) },
                    { 3.0 / 5.0, leds::Color(255, 0, 255) },
                    { 4.0 / 5.0, leds::Color(255, 0, 0) },
            }));
        }

    } // namespace ledsigns::common
} // namespace ledsigns
