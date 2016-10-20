/**
* @file
*
* Implementation for the bbox::rt::NetworkChangeRef class.
*/

#ifdef WIN32

#include <bbox/rt/NetworkChangeRef.h>
#include <bbox/Assert.h>

namespace bbox {
    namespace rt {

        NetworkChangeRef::NetworkChangeRef(const std::string &name, Service &parent)
            : ServiceReference<win32::NetworkChangeService>(name, parent, win32::NetworkChangeService::SERVICE_NAME)
        {
        }

        NetworkChangeRef::~NetworkChangeRef()
        {
        }

        std::map<std::string, net::AdapterInfo> NetworkChangeRef::GetCurrentAdapterInfo()
        {
            BBOX_ASSERT((GetLocalRunLevel() == RunLevel::RUNNING) || (GetLocalRunLevel() == RunLevel::STOPPING));

            return (*this)->GetCurrentAdapterInfo();
        }

    } // namespace bbox::rt
} // namespace bbox

#endif // WIN32
