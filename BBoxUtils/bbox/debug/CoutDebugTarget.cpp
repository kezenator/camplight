/**
* @file
*
* Implementation file for the bbox::debug::CoutDebugTarget class.
*/

#include <iostream>
#include <bbox/debug/CoutDebugTarget.h>
#include <boost/bind.hpp>

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
            std::cout << report.Data();
            std::cout.flush();
        }
    }
}
