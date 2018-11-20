/**
 * @file
 *
 * Header for the bbox::rt::1inux::NetworkChangeService class.
 */

#ifndef __BBOX__RT__LIN__LINUX_NETWORK_CHANGE_SERVICE_H__
#define __BBOX__RT__LIN__LINUX_NETWORK_CHANGE_SERVICE_H__

#pragma once

#include <bbox/rt/Service.h>
#include <bbox/rt/ThreadPoolRef.h>
#include <bbox/rt/Timer.h>
#include <bbox/rt/net/AdapterInfo.h>

#include <bbox/rt/net/NetworkChangeService.h>

namespace bbox {
    namespace rt {
        namespace lin {

            /**
             * A service that detects networks adapters connected to the PC.
             */
            class LinuxNetworkChangeService : public net::NetworkChangeService
            {
            public:

                LinuxNetworkChangeService(const std::string &name, Proactor &parent);
                LinuxNetworkChangeService(const std::string &name, Service &parent);
                virtual ~LinuxNetworkChangeService();

            private:
                void HandleStarting() override;
                void HandleStopping() override;

                void CheckShutdown();
                void HandleDetectionTimerExpired();
                void DoDetectionInThreadPool();
                void HandleDetectionCompleted();

                ThreadPoolRef m_thread_pool_ref;
                Timer m_detection_timer;
                bool m_detection_in_progress;

                std::map<std::string, net::AdapterInfo> m_detecting_adapters;
            };

        } // namespae bbox::rt::lin
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__LIN__LINUX_NETWORK_CHANGE_SERVICE_H__