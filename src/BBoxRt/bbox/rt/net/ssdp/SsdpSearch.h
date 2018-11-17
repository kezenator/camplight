/**
 * @file
 *
 * Header for the bbox::rt::net::SsdpSearch class.
 */

#ifndef __BBOX__RT__NET__SSDP__SSDP_SEARCH_H__
#define __BBOX__RT__NET__SSDP__SSDP_SEARCH_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/ServiceReference.h>
#include <bbox/rt/Timer.h>
#include <bbox/rt/OneShotWork.h>
#include <bbox/Uuid.h>
#include <bbox/rt/net/IpAddress.h>
#include <bbox/rt/net/ssdp/DeviceInfo.h>

namespace bbox {
namespace rt {
namespace net {
namespace ssdp {

// Forward declaration
class SsdpDiscoveryService;

/**
 * A class that advertises a service via Upnp, and discovers
 * other services.
 */
class SsdpSearch : public Service
{
    friend class SsdpDiscoveryService;

public:

    using ChangeHandler = std::function<void()>;

    SsdpSearch(const std::string &name, Service &parent, std::string &&service_type, ChangeHandler &&handler);
    ~SsdpSearch();

    const std::map<std::string, DeviceInfo> &GetCurrentDeviceMap() const
    {
        return m_devices;
    }

private:
    void HandleStarting() override;
    void HandleStopping() override;
    void PrintState(bbox::DebugOutput &out) const override;

    void RestartAdvertTimeOnNetworkChange();
    void RemoveDevice(const std::string &usn);
    void RemoveInterface(const IpAddress &local_addr);
    void HandleNotification(const IpAddress &local_addr, const IpAddress &device_addr, const std::string &usn, const std::string &search_type, const std::string &location);

    void HandleTimerExpired();
    void HandleDevicesChanged();

    ServiceReference<SsdpDiscoveryService> m_discovery_service_ref;
    Timer m_timer;
    OneShotWork m_change_work;
    const Uuid m_uuid;
    const std::string m_service_type;
    const ChangeHandler m_handler;
    const size_t m_search_duration;
    size_t m_announce_count;
    size_t m_search_counter;
    std::map<std::string, DeviceInfo> m_devices;
};

} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__SSDP__SSDP_SEARCH_H__
