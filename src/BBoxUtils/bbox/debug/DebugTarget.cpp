/**
 * @file
 *
 * Implementation file for the bbox::debug::DebugTarget class.
 */

#include <bbox/debug/DebugTarget.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/Assert.h>

namespace bbox
{
	namespace debug
	{

		DebugTarget::DebugTarget()
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);
			DebugProvider::t_provider_ptr->LoginTarget(this);
		}

		DebugTarget::~DebugTarget()
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);
			DebugProvider::t_provider_ptr->LogoutTarget(this);
		}

	}
}
