/**
* @file
*
* Implementation for the bbox::rt::DebugEnable class.
*/

#include <bbox/rt/DebugEnable.h>

namespace bbox {
    namespace rt {

        DebugEnable::DebugEnable(const std::string &name, Service &parent)
            : Resource(name, parent)
            , m_enabled(false)
        {
        }

        DebugEnable::DebugEnable(const std::string &name, Resource &parent)
            : Resource(name, parent)
            , m_enabled(false)
        {
        }
        
        DebugEnable::~DebugEnable()
        {
        }

        void DebugEnable::HandleStopping()
        {
            RequestStopAllChildren();
            NotifyStopped();
        }

        void DebugEnable::DebugVisit(bbox::debug::DebugVisitor &&visitor)
        {
            if (visitor.WantControlDebugEnable())
            {
                m_enabled = visitor.GetDebugEnableState();
            }
                
            Resource::DebugVisit(std::move(visitor));
        }

        void DebugEnable::PrintState(bbox::DebugOutput &out) const
        {
            out.Format("%s\n", m_enabled);
        }

    } // namespace bbox::rt
} // namespace bbox
