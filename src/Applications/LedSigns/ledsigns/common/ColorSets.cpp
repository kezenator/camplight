/**
 * @file
 *
 * Implementation file for the ledsigns::common::ColorSets class.
 */

#include <ledsigns/common/ColorSets.h>

namespace ledsigns {
namespace common {

std::vector<leds::Color> ColorSets::FullRainbow()
{
    return
    {
        leds::Color(255, 0, 0),
        leds::Color(255, 128, 0),
        leds::Color(255, 255, 0),
        leds::Color(0, 255, 0),
        leds::Color(0, 255, 255),
        leds::Color(0, 0, 255),
        leds::Color(128, 0, 255),
        leds::Color(255, 0, 255),
   };
}

std::vector<leds::Color> ColorSets::GreensAndBlues()
{
    return
    {
        leds::Color(0, 196, 64),
        leds::Color(0, 255, 255),
        leds::Color(0, 0, 255),
        leds::Color(96, 0, 128),
    };
}

std::vector<leds::Color> ColorSets::RedsAndPurples()
{
    return
    {
        leds::Color(0, 255, 255),
        leds::Color(0, 0, 255),
        leds::Color(128, 0, 255),
        leds::Color(255, 0, 255),
        leds::Color(255, 0, 0),
    };
}

} // namespace ledsigns::common
} // namespace ledsigns
