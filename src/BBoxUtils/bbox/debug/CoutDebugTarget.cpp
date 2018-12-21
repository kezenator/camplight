/**
* @file
*
* Implementation file for the bbox::debug::CoutDebugTarget class.
*/

#include <iostream>
#include <bbox/debug/CoutDebugTarget.h>
#include <boost/bind.hpp>

#ifdef WIN32
#else // not WIN32
#include <bbox/Assert.h>
#include <bbox/Format.h>
#endif // not WIN32

namespace bbox
{
    namespace debug
    {
		CoutDebugTarget::CoutDebugTarget()
            : DebugTarget()
        {
        }

		CoutDebugTarget::~CoutDebugTarget()
        {
        }

        void CoutDebugTarget::OutputToTarget(bbox::debug::DebugReport report)
        {
#ifdef WIN32
            // TODO - print date/time
#else // not WIN32
            struct timespec tp;

            auto result = clock_gettime(CLOCK_REALTIME, &tp);
            BBOX_ASSERT(result == 0);

            time_t wallt = tp.tv_sec;
            struct tm local_wall;
            localtime_r(&wallt, &local_wall);

            char buffer[128];
            strftime(buffer, sizeof(buffer), "@ %F %T.", &local_wall);
            std::cout << buffer;
            std::cout << Format("%06d", tp.tv_nsec / 1000);
            strftime(buffer, sizeof(buffer), " %z\n", &local_wall);
            std::cout << buffer;

#endif // not WIN32

            std::cout << report.Data();
            std::cout.flush();
        }
    }
}
