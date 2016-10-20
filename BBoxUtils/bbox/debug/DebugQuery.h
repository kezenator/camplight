/**
 * @file
 *
 * Header file for the bbox::debug::DebugQuery class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_QUERY_H__
#define __BBOX__DEBUG__DEBUG_QUERY_H__

#include <string>
#include <vector>
#include <bbox/debug/DebugQueryResult.h>

namespace bbox
{
	namespace debug
	{

		/**
		 * Performs queries into the debug system.
		 */
		class DebugQuery
		{
			DebugQuery() = delete;
			DebugQuery(const DebugQuery &) = delete;
			const DebugQuery &operator =(const DebugQuery &) = delete;

		public:

			static std::vector<DebugQueryResult> DoQuery(const std::string &query);
		};

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_QUERY_H__
