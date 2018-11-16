/**
* @file
*
* Implementation for the bbox::rt::1inux::LinuxNetworkChangeService class.
*/

#ifndef WIN32

#include <bbox/rt/lin/LinuxNetworkChangeService.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>

namespace bbox {
    namespace rt {
        namespace lin {

            LinuxNetworkChangeService::LinuxNetworkChangeService(const std::string &name, Proactor &parent)
                : net::NetworkChangeService(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_detection_timer("detection-timer", *this)
                , m_detection_in_progress(false)
                , m_detecting_adapters()
            {
            }

            LinuxNetworkChangeService::LinuxNetworkChangeService(const std::string &name, Service &parent)
                : net::NetworkChangeService(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_detection_timer("detection-timer", *this)
                , m_detection_in_progress(false)
                , m_detecting_adapters()
            {
            }

            LinuxNetworkChangeService::~LinuxNetworkChangeService()
            {
            }

            void LinuxNetworkChangeService::HandleStarting()
            {
                BBOX_ASSERT(m_detection_in_progress == false);

                m_detection_timer.SetHandler(std::bind(&LinuxNetworkChangeService::HandleDetectionTimerExpired, this));

                m_detection_in_progress = true;
                m_thread_pool_ref.RunOnThread(
                        boost::bind(&LinuxNetworkChangeService::DoDetectionInThreadPool, this),
                        boost::bind(&LinuxNetworkChangeService::HandleDetectionCompleted, this));
            }

            void LinuxNetworkChangeService::HandleStopping()
            {
                CheckShutdown();
            }

            void LinuxNetworkChangeService::CheckShutdown()
            {
                if ((GetLocalRunLevel() == RunLevel::STOPPING)
                    && !m_detection_in_progress)
                {
                    m_detection_timer.Cancel();

                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

            void LinuxNetworkChangeService::PrintState(bbox::DebugOutput &out) const
            {
                out.Format("Current adapters: %d\n", GetCurrentAdapterInfo().size());

                ScopedDebugIndent indent(out, 4);

                for (const auto &entry : GetCurrentAdapterInfo())
                {
                    out.Format("\n");
                    out.Format("Name: %s\n", entry.first);
                    out.Format("System Name: %s\n", entry.second.system_name);
                    out.Format("User Name: %s\n", entry.second.user_name);
                    out.Format("Description: %s\n", entry.second.description);
                    out.Format("MAC Address: %s\n", entry.second.mac_address);

                    for (const std::string &addr : entry.second.ip_addresses)
                        out.Format("IP Address: %s\n", addr);
                }
            }

            void LinuxNetworkChangeService::HandleDetectionTimerExpired()
            {
                BBOX_ASSERT(m_detection_in_progress == false);
                BBOX_ASSERT((GetLocalRunLevel() == RunLevel::STARTING)
                    || (GetLocalRunLevel() == RunLevel::RUNNING));

                m_detection_in_progress = true;
                m_thread_pool_ref.RunOnThread(
                        boost::bind(&LinuxNetworkChangeService::DoDetectionInThreadPool, this),
                        boost::bind(&LinuxNetworkChangeService::HandleDetectionCompleted, this));
            }

            void LinuxNetworkChangeService::DoDetectionInThreadPool()
            {
                m_detecting_adapters.clear();

                struct ifaddrs *ifa = nullptr;

                int result = getifaddrs(&ifa);

                if (result == 0)
                {
                    for (;
                        ifa != nullptr;
                        ifa = ifa->ifa_next)
                    {
                        if ((ifa->ifa_addr->sa_family==AF_INET)
                            || (ifa->ifa_addr->sa_family==AF_PACKET))
                        {
                            const std::string name { ifa->ifa_name};

                            {
                                auto it = m_detecting_adapters.find(name);

                                if (it == m_detecting_adapters.end())
                                {
                                    net::AdapterInfo &new_info = m_detecting_adapters[ifa->ifa_name];

                                    new_info.system_name = name;
                                    new_info.user_name = name;
                                    new_info.description = name;
                                }
                            }

                            if (ifa->ifa_addr->sa_family==AF_INET)
                            {
                                m_detecting_adapters[name].ip_addresses.insert(inet_ntoa(reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr)->sin_addr));
                            }
                            else if ((ifa->ifa_addr->sa_family==AF_PACKET)
                                && (reinterpret_cast<struct sockaddr_ll *>(ifa->ifa_addr)->sll_halen == 6))
                            {
                                auto ptr = reinterpret_cast<struct sockaddr_ll *>(ifa->ifa_addr)->sll_addr;
                                m_detecting_adapters[name].mac_address = bbox::Format("%02X:%02X:%02X:%02X:%02X:%02X",
                                    ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);

                            }
                        }
                    }
                }

                freeifaddrs(ifa);
            }

            void LinuxNetworkChangeService::HandleDetectionCompleted()
            {
                BBOX_ASSERT(m_detection_in_progress == true);
                m_detection_in_progress = false;

                m_detection_timer.StartSingleShot(TimeSpan::Seconds(10));

                if ((m_detecting_adapters != GetCurrentAdapterInfo())
                    || (GetLocalRunLevel() == RunLevel::STARTING))
                {
                    // We've got new settings - save them, and either
                    // mark us as started or post a change notification

                    {
                        std::cout << "Network Adapters - "
                            << ((GetLocalRunLevel() == RunLevel::STARTING) ? "Initial State" : "Updated")
                            << std::endl;

                        for (const auto &entry : m_detecting_adapters)
                        {
                            const net::AdapterInfo &adapter = entry.second;

                            std::cout << "   Adapter " << adapter.system_name << std::endl;
                            std::cout << "      User Name    : " << adapter.user_name << std::endl;
                            std::cout << "      Description  : " << adapter.description << std::endl;
                            std::cout << "      MAC Address  : " << adapter.mac_address << std::endl;

                            for (const std::string &address: adapter.ip_addresses)
                                std::cout << "      IP Address   : " << address << std::endl;
                        }
                    }

                    ReportChange(std::move(m_detecting_adapters));

                    if (GetLocalRunLevel() == RunLevel::STARTING)
                    {
                        NotifyStarted();
                    }
                }

                CheckShutdown();
            }

        } // namespace bbox::rt::lin
    } // namespace bbox::rt
} // namespace bbox

#endif // not WIN32