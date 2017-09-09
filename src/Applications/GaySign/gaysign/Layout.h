/**
 * @file
 *
 * Header file for the gaysign::Layout class.
 */

#ifndef __GAYSIGN__LAYOUT_H__
#define __GAYSIGN__LAYOUT_H__

#include <vector>

namespace gaysign
{

    /**
     * Describes the layout of the LEDs in the box.
     */
    class Layout
    {
    public:
        struct Entry
        {
            bool symbol;
            double x;
            double y;
        };

        static constexpr size_t NUM_LEDS = 11;

        static const std::vector<Entry> ENTRIES;
    };

} // namespace gaysign

#endif // __GAYSIGN__LAYOUT_H__
