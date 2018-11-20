/**
* @file
*
* Implementation for the bbox::rt::1inux::LinuxNetworkChangeService class.
*/

#ifndef WIN32

#include <bbox/rt/lin/LinuxNetworkChangeService.h>
#include <bbox/Assert.h>

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
                SetThisDependantOn(m_thread_pool_ref);
            }

            LinuxNetworkChangeService::LinuxNetworkChangeService(const std::string &name, Service &parent)
                : net::NetworkChangeService(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_detection_timer("detection-timer", *this)
                , m_detection_in_progress(false)
                , m_detecting_adapters()
            {
                SetThisDependantOn(m_thread_pool_ref);
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
                                net::AdapterAddressInfo addr_info;

                                addr_info.address =
                                    boost::asio::ip::address(boost::asio::ip::address_v4(
                                        ntohl(reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr)->sin_addr.s_addr)));
                                addr_info.broadcast =
                                    boost::asio::ip::address(boost::asio::ip::address_v4(
                                        ntohl(reinterpret_cast<struct sockaddr_in *>(ifa->ifa_broadaddr)->sin_addr.s_addr)));

                                m_detecting_adapters[name].ip_addresses.push_back(addr_info);
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

                ReportChange(std::move(m_detecting_adapters));

                if (GetLocalRunLevel() == RunLevel::STARTING)
                {
                    NotifyStarted();
                }

                CheckShutdown();
            }

        } // namespace bbox::rt::lin
    } // namespace bbox::rt
} // namespace bbox

#endif // not WIN32
