/**
 * @file
 *
 * Implementation file for the ledsigns::common::Graident class.
 */

#include <ledsigns/common/Gradient.h>
#include <ledsigns/common/VectorGradient.h>
#include <ledsigns/common/ColorSets.h>

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
            return VectorGradient::EqualRing("Full-Rainbow", ColorSets::FullRainbow());
        }

        Gradient::Ptr Gradient::GreensAndBlues()
        {
            return VectorGradient::EqualRing("Greens-and-Blues", ColorSets::GreensAndBlues());
        }

        Gradient::Ptr Gradient::RedsAndPurples()
        {
            return VectorGradient::EqualRing("Reds-and-Purples", ColorSets::RedsAndPurples());
        }

        Gradient::Ptr Gradient::CasaDelShadePulseReds()
        {
            return std::make_unique<VectorGradient>(
                "Pulse-Reds",
                common::VectorGradient::Map({
                    { 0.0, leds::Color(64, 0, 0) },
                    { 0.3, leds::Color(255, 0, 0) },
                    { 0.8, leds::Color(255, 128, 0) },
                    { 1.0, leds::Color(200, 200, 200) },
                }));
        }

    } // namespace ledsigns::common
} // namespace ledsigns
