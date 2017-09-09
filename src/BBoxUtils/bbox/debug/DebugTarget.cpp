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
            : m_provider_ptr(nullptr)
		{
			BBOX_ASSERT(DebugProvider::t_provider_ptr);

            m_provider_ptr = DebugProvider::t_provider_ptr;
			m_provider_ptr->LoginTarget(this);
		}

		DebugTarget::~DebugTarget()
		{
            BBOX_ASSERT(DebugProvider::t_provider_ptr);
            BBOX_ASSERT(m_provider_ptr);
            BBOX_ASSERT(m_provider_ptr == DebugProvider::t_provider_ptr);

            m_provider_ptr = nullptr;
            DebugProvider::t_provider_ptr->LogoutTarget(this);
		}

        void DebugTarget::UpdateDebugEnables(std::set<std::string> &&debug_enables)
        {
            if (debug_enables != m_debug_enables)
            {
                m_debug_enables = std::move(debug_enables);

                if (m_provider_ptr)
                {
                    m_provider_ptr->TargetDebugEnablesUpdated();
                }
            }
        }

	}
}
