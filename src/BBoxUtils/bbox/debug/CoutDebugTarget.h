/**
* @file
*
* Header file for the bbox::debug::CoutDebugTarget class.
*/

#ifndef __BBOX__DEBUG__COUT_DEBUG_TARGET_H__
#define __BBOX__DEBUG__COUT_DEBUG_TARGET_H__

#include <bbox/debug/DebugTarget.h>

namespace bbox
{
    namespace debug
    {
        /**
         * A debug target that outputs to std::cout.
         */
        class CoutDebugTarget : public DebugTarget
        {
        public:
			CoutDebugTarget();
            ~CoutDebugTarget();

            void OutputToTarget(bbox::debug::DebugReport report) override;
        };
    }
}

#endif // __BBOX__DEBUG__COUT_DEBUG_TARGET_H__
