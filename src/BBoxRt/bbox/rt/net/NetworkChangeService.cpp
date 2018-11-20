/**
* @file
*
* Implementation for the bbox::rt::net::NetworkChangeService class.
*/

#include <bbox/rt/net/NetworkChangeService.h>
#include <bbox/rt/net/NetworkChangeHandler.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>
#include <bbox/enc/ToDebugOutput.h>

namespace bbox {
    namespace rt {
        namespace net {

            const char *NetworkChangeService::SERVICE_NAME = "win32/NetworkChangeService";

            NetworkChangeService::NetworkChangeService(const std::string &name, Proactor &parent)
                : Service(name, parent)
                , m_current_adapters()
                , m_handlers()
                , m_debug_enable("change-debug", *this)
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::NetworkChangeService(const std::string &name, Service &parent)
                : Service(name, parent)
                , m_current_adapters()
                , m_handlers()
                , m_debug_enable("change-debug", *this)
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::~NetworkChangeService()
            {
                BBOX_ASSERT(m_handlers.empty());
            }

            void NetworkChangeService::PrintState(bbox::DebugOutput &out) const
            {
                out.Format("Current adapters: %d\n", m_current_adapters.size());

                ScopedDebugIndent indent(out, 4);
                bbox::enc::ToDebugOutput(out, m_current_adapters);
            }

            void NetworkChangeService::ReportChange(std::map<std::string, net::AdapterInfo> &&new_adapters)
            {
                if ((new_adapters != m_current_adapters)
                    || (GetLocalRunLevel() == RunLevel::STARTING))
                {
                    // We've got new settings - save them and notify the handlers

                    bbox::DebugOutput out(BBOX_FUNC, m_debug_enable);
                    if (out)
                    {
                        out << "Network Adapters - "
                            << ((GetLocalRunLevel() == RunLevel::STARTING) ? "Initial State" : "Updated")
                            << std::endl;

                        ScopedDebugIndent indent(out, 4);
                        bbox::enc::ToDebugOutput(out, new_adapters);
                    }

                    m_current_adapters = std::move(new_adapters);

                    std::vector<std::function<void()>> funcs_to_run;
                    funcs_to_run.reserve(m_handlers.size());

                    for (NetworkChangeHandler *handler : m_handlers)
                    {
                        funcs_to_run.push_back(handler->m_change_handler);
                    }

                    for (const auto &func : funcs_to_run)
                    {
                        func();
                    }
                }
            }

        } // namespace bbox::rt::net
    } // namespace bbox::rt
} // namespace bbox
