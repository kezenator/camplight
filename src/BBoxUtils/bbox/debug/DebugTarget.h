/**
 * @file
 *
 * Header file for the bbox::debug::DebugTarget class.
 */

#ifndef __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__
#define __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__

#include <string>
#include <set>
#include <bbox/debug/DebugReport.h>

namespace bbox
{
	namespace debug
	{
        // Forward declarations
        class DebugProvider;

		/**
         * A target where debug output is sent.
 		 */
		class DebugTarget
		{
            friend class DebugProvider;

			DebugTarget(const DebugTarget &) = delete;
			const DebugTarget &operator =(const DebugTarget &) = delete;

		public:

			virtual void OutputToTarget(bbox::debug::DebugReport report) = 0;

		protected:
			DebugTarget();
			virtual ~DebugTarget();

            void UpdateDebugEnables(std::set<std::string> &&debug_enables);

        private:
            DebugProvider *m_provider_ptr;
            std::set<std::string> m_debug_enables;
		};
	}
}

#endif // __BBOX__DEBUG__DEBUG_OUTPUT_TARGET_H__
