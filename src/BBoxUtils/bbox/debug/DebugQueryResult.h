/**
 * @file
 *
 * Header file for the bbox::debug::DebugQueryResult class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_QUERY_RESULT_H__
#define __BBOX__DEBUG__DEBUG_QUERY_RESULT_H__

#include <string>
#include <vector>
#include <bbox/debug/DebugChildEntry.h>
#include <bbox/debug/DebugReport.h>

namespace bbox
{
	namespace debug
	{

		/**
		 * Represents a single result from a debug query.
		 */
		class DebugQueryResult
		{
		public:
            std::string parent_path;
			std::string full_path;
            DebugReport report;
			std::vector<DebugChildEntry> children;
		};

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_QUERY_RESULT_H__
