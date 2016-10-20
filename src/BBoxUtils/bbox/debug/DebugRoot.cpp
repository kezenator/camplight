/**
 * @file
 *
 * Implementation file for the bbox::debug::DebugRoot class.
 *
 */


#include <bbox/debug/DebugRoot.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/Assert.h>

namespace bbox
{
	namespace debug
	{

		DebugRoot::DebugRoot(std::string &&name)
			: m_name(std::move(name))
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);
			DebugProvider::t_provider_ptr->LoginRoot(this);
		}

		DebugRoot::~DebugRoot()
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);
			DebugProvider::t_provider_ptr->LogoutRoot(this);
		}

	} // namespace bbox::debug
} // namespace bbox
