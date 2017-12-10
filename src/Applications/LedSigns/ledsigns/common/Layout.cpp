/**
 * @file
 *
 * Implementation for the ledsigns::common::Layout class.
 *
 */

#include <ledsigns/common/Layout.h>

namespace ledsigns
{
    namespace common
    {
        Layout::Layout(const char *_name, const std::initializer_list<Entry> &_entries)
            : name(_name)
            , num_leds(_entries.size())
            , entries(_entries)
        {
        }

        Layout::~Layout()
        {
        }

    } // namespace ledsigns::common
} // namespace ledsigns
