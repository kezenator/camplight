/**
* @file
*
* Implementation for the bbox::rt::net::NetworkChangeService class.
*/

#include <bbox/rt/net/NetworkChangeService.h>
#include <bbox/Assert.h>

namespace bbox {
    namespace rt {
        namespace net {

            const char *NetworkChangeService::SERVICE_NAME = "win32/NetworkChangeService";

            NetworkChangeService::NetworkChangeService(const std::string &name, Proactor &parent)
                : Service(name, parent)
                , m_current_adapters()
                , m_handlers()
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::NetworkChangeService(const std::string &name, Service &parent)
                : Service(name, parent)
                , m_current_adapters()
                , m_handlers()
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::~NetworkChangeService()
            {
                BBOX_ASSERT(m_handlers.empty());
            }

            void NetworkChangeService::ReportChange(std::map<std::string, net::AdapterInfo> &&new_adapters)
            {
                m_current_adapters = std::move(new_adapters);

                // TODO - report to user
            }

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox
