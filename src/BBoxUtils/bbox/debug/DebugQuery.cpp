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
#include <bbox/TypeInfo.h>

namespace bbox
{
	namespace debug
	{

		std::vector<DebugQueryResult> DebugQuery::DoQuery(const std::string &query)
		{
            DebugProvider *provider_ptr = DebugProvider::t_provider_ptr;
            
            BBOX_ASSERT(provider_ptr);

			std::vector<DebugQueryResult> results;

			{
				DebugVisitor root_visitor(results, query);

                provider_ptr->RootQuery(std::move(root_visitor));
			}

			return results;
		}

        void DebugQuery::DoDebugEnableQuery(const std::set<std::string> &debug_enables)
        {
            DebugProvider *provider_ptr = DebugProvider::t_provider_ptr;

            BBOX_ASSERT(provider_ptr);

            DebugVisitor root_visitor(debug_enables);

            provider_ptr->RootQuery(std::move(root_visitor));
        }

	} // namespace bbox::debug
} // namespace bbox
