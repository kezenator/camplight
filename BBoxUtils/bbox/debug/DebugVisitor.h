/**
 * @file
 *
 * Header file for the bbox::debug::DebugVisitor class.
 *
 */

#ifndef __BBOX__DEBUG__DEBUG_VISITOR_H__
#define __BBOX__DEBUG__DEBUG_VISITOR_H__

#include <string>
#include <memory>
#include <vector>
#include <bbox/debug/DebugReport.h>

namespace bbox
{
	namespace debug
	{
		// Forward declarations
		class DebugQuery;
		class DebugQueryResult;

		/**
		 * A visitor context that allows users to efficiently
		 * walk the tree of debug items for the searched items
		 * and return results for the requested items.
		 */
		class DebugVisitor
		{
			friend class DebugQuery;

			DebugVisitor() = delete;
			DebugVisitor(const DebugVisitor &) = delete;
			const DebugVisitor &operator =(const DebugVisitor &) = delete;

		public:

			DebugVisitor(DebugVisitor &&other) = default;
			DebugVisitor &operator =(DebugVisitor &&other) = default;
			~DebugVisitor();

			bool WantVisitChildren();
			bool WantReport();

			DebugVisitor ChildVisitor(const std::string &child_name);

			void SetReport(DebugReport report);

		private:

			struct Pimpl;

			std::unique_ptr<Pimpl> m_pimpl;

			DebugVisitor(std::unique_ptr<Pimpl> &&pimpl);

			DebugVisitor(std::vector<DebugQueryResult> &results, const std::string &query);
		};

	} // namespace bbox::debug
} // namespace bbox

#endif // __BBOX__DEBUG__DEBUG_VISITOR_H__
