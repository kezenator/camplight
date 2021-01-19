/**
 * @file
 *
 * Header file for the ledsigns::common::ColorSets class.
 */

#ifndef __LEDSIGNS__COMMON__COLOR_SETS_H__
#define __LEDSIGNS__COMMON__COLOR_SETS_H__

#include <map>
#include <vector>
#include <leds/Color.h>

namespace ledsigns {
namespace common {

/**
 * Provides a set of pre-definied sets of colors.
 */
class ColorSets
{
public:

    static std::vector<leds::Color> FullRainbow();
    static std::vector<leds::Color> GreensAndBlues();
    static std::vector<leds::Color> RedsAndPurples();
};

} // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__COLOR_SETS_H__
