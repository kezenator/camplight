/**
 * @file
 *
 * Implementation file for the bbox::debug::DebugQuery class.
 *
 */

#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/DebugQuery.h>
#include <bbox/debug/DebugRoot.h>
#include <bbox/debug/DebugVisitor.h>
#include <bbox/Assert.h>

namespace bbox
{
	namespace debug
	{

		std::vector<DebugQueryResult> DebugQuery::DoQuery(const std::string &query)
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);

			std::vector<DebugQueryResult> results;

			{
				DebugVisitor root_visitor(results, query);

				if (root_visitor.WantVisitChildren())
				{
					auto &root_map = DebugProvider::t_provider_ptr->GetRootMap();

					for (const auto &entry : root_map)
					{
						entry.second->VisitDebugRootEntry(root_visitor.ChildVisitor(entry.first));
					}
				}

				if (root_visitor.WantReport())
				{
                    bbox::debug::DebugReport report;

                    {
                        bbox::DebugOutput out(BBOX_FUNC, report, "Root query");
                    }

					root_visitor.SetReport(report);
				}
			}

			return results;
		}

	} // namespace bbox::debug
} // namespace bbox
