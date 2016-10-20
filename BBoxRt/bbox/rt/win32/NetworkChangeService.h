/**
 * @file
 *
 * Header for the bbox::rt::win32::NetworkChangeService class.
 */

#ifndef __BBOX__RT__WIN32__NETWORK_CHANGE_SERVICE_H__
#define __BBOX__RT__WIN32__NETWORK_CHANGE_SERVICE_H__

#pragma once

#include <bbox/rt/Service.h>
#include <bbox/rt/ThreadPoolRef.h>
#include <bbox/rt/net/AdapterInfo.h>

#include <boost/atomic.hpp>

namespace bbox {
    namespace rt { 
        namespace win32 {

            /**
             * A service that detects networks adapters connected to the PC.
             */
            class NetworkChangeService : public Service
            {
            public:

                static const char *SERVICE_NAME;

                NetworkChangeService(const std::string &name, Proactor &parent);
                NetworkChangeService(const std::string &name, Service &parent);
                virtual ~NetworkChangeService();

                void TriggerUpdate();

                const std::map<std::string, net::AdapterInfo> &GetCurrentAdapterInfo() const
                {
                    return m_current_adapters;
                }

            private:
                void HandleStarting() override;
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;

                void CheckShutdown();
                void HandleUpdateRequired();
                void HandlUpdateComplete();
                void RequestAdapters();

                ThreadPoolRef m_thread_pool_ref;
                void *m_change_handle;
                boost::atomic<size_t> m_pending_counter;
                bool m_another_update_required;

                std::map<std::string, net::AdapterInfo> m_current_adapters;
                std::map<std::string, net::AdapterInfo> m_detecting_adapters;
            };

        } // namespae bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__WIN32__NETWORK_CHANGE_SERVICE_H__