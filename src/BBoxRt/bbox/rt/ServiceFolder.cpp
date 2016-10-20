/**
* @file
*
* Implementation for the bbox::rt::ServiceFolder class.
*/

#include <bbox/rt/ServiceFolder.h>

namespace bbox {
    namespace rt {

        ServiceFolder::ServiceFolder(const std::string &name, Proactor &parent)
            : Service(name, parent)
        {
        }

        ServiceFolder::ServiceFolder(const std::string &name, Service &parent)
            : Service(name, parent)
        {
        }

        ServiceFolder::~ServiceFolder()
        {
        }

        void ServiceFolder::HandleStarting()
        {
            NotifyStarted();
        }

        void ServiceFolder::HandleStopping()
        {
            NotifyStopped();
            RequestStopAllChildren();
        }
		
		void ServiceFolder::PrintState(bbox::DebugOutput & /*out*/) const
		{
		}

    } // namespace bbox::rt
} // namespace bbox
