/**
 * @file
 *
 * Header for the bbox::rt::win32::NetworkChangeService class.
 */

#ifndef __BBOX__RT__WIN32__WIN32_NETWORK_CHANGE_SERVICE_H__
#define __BBOX__RT__WIN32__WIN32_NETWORK_CHANGE_SERVICE_H__

#pragma once

#include <bbox/rt/Service.h>
#include <bbox/rt/ThreadPoolRef.h>
#include <bbox/rt/net/AdapterInfo.h>

#include <boost/atomic.hpp>

#include <bbox/rt/net/NetworkChangeService.h>

namespace bbox {
    namespace rt { 
        namespace win32 {

            /**
             * A service that detects networks adapters connected to the PC.
             */
            class Win32NetworkChangeService : public net::NetworkChangeService
            {
            public:

				Win32NetworkChangeService(const std::string &name, Proactor &parent);
				Win32NetworkChangeService(const std::string &name, Service &parent);
                virtual ~Win32NetworkChangeService();

                void TriggerUpdate();

            private:
                void HandleStarting() override;
                void HandleStopping() override;

                void CheckShutdown();
                void HandleUpdateRequired();
                void HandlUpdateComplete();
                void RequestAdapters();

                ThreadPoolRef m_thread_pool_ref;
                void *m_change_handle;
                boost::atomic<size_t> m_pending_counter;
                bool m_another_update_required;

                std::map<std::string, net::AdapterInfo> m_detecting_adapters;
            };

        } // namespae bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__WIN32__WIN32_NETWORK_CHANGE_SERVICE_H__