/**
 * @file
 *
 * Header file for the bbox::debug::DebugTarget class.
 */

#ifndef __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__
#define __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__

#include <string>
#include <bbox/debug/DebugReport.h>

namespace bbox
{
	namespace debug
	{
		/**
         * A target where debug output is sent.
 		 */
		class DebugTarget
		{
			DebugTarget(const DebugTarget &) = delete;
			const DebugTarget &operator =(const DebugTarget &) = delete;

		public:

			virtual void OutputToTarget(bbox::debug::DebugReport report) = 0;

		protected:
			DebugTarget();
			virtual ~DebugTarget();
		};
	}
}

#endif // __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__
