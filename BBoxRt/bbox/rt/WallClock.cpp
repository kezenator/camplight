/**
* @file
*
* Implementation for the bbox::rt::WallClock class.
*/

#include <bbox/rt/WallClock.h>
#include <bbox/Assert.h>

#include <time.h>

namespace bbox {
    namespace rt {

        std::string WallClock::GetCurrentTimeRfc1123()
        {
            // Gets the current time as a RFS1123 time -
            // e.g. Mon, 15 Jun 2009 20:45:30 GMT

            // First, get the stuff that strftime can do

            char buffer[30];

#if WIN32
            __time64_t t;

            _time64(&t);
            struct tm *tm_ptr = _gmtime64(&t);
#else
            time_t time_val = time(nullptr);
            BBOX_ASSERT(time_val != time_t(-1));

            struct tm *tm_ptr = gmtime(&time_val);
#endif

            strftime(buffer, sizeof(buffer), "%d  %Y %H:%M:%S GMT", tm_ptr);

            std::string result(buffer);

            // Now, insert the month after the first space

            size_t first_space_pos = result.find(' ');

            static const char * const months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

            result.insert(first_space_pos + 1, months[tm_ptr->tm_mon]);

            // Insert the day of week at the start, followed by a
            // comma and space

            result.insert(0, ", ");

            static const char * const days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

            result.insert(0, days[tm_ptr->tm_wday]);

            return result;
        }

    } // namespace bbox::rt
} // namespace bbox
