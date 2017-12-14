/**
 * @file
 *
 * Header file for the ledsigns::common::Layout class.
 */

#ifndef __LEDSIGNS__COMMON__LAYOUT_H__
#define __LEDSIGNS__COMMON__LAYOUT_H__

#include <string>
#include <vector>
#include <cstddef>

namespace ledsigns
{
    namespace common
    {

        /**
         * Describes the layout of the LEDs in the box.
         */
        class Layout
        {
            Layout() = delete;
            Layout(const Layout &) = delete;
            Layout &operator =(const Layout &) = delete;

        public:

            struct Entry
            {
                int symbol_num;
                double x;
                double y;
            };

            Layout(const char *_name, const std::initializer_list<Entry> &_entries);
            ~Layout();

            const std::string name;
            const size_t num_leds;
            const std::vector<Entry> entries;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__LAYOUT_H__
