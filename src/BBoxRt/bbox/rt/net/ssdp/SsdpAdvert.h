/**
 * @file
 *
 * Header for the bbox::rt::net::SsdpAdvert class.
 */

#ifndef __BBOX__RT__NET__SSDP__SSDP_ADVERT_H__
#define __BBOX__RT__NET__SSDP__SSDP_ADVERT_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/ServiceReference.h>
#include <bbox/rt/Timer.h>
#include <bbox/Uuid.h>
#include <bbox/rt/net/IpAddress.h>

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
class SsdpAdvert : public Service
{
    friend class SsdpDiscoveryService;

public:

    using InfoCallback = std::function<void(const bbox::rt::net::IpAddress &interface_addr, bool &out_should_send, std::string &out_location)>;

    SsdpAdvert(const std::string &name, Service &parent, std::string &&service_type, InfoCallback &&info_callback);
    ~SsdpAdvert();

private:
    void HandleStarting() override;
    void HandleStopping() override;
    void PrintState(bbox::DebugOutput &out) const override;

    void RestartAdvertTimeOnNetworkChange();

    void HandleTimerExpired();

    ServiceReference<SsdpDiscoveryService> m_discovery_service_ref;
    Timer m_timer;
    const Uuid m_uuid;
    const std::string m_service_type;
    const InfoCallback m_info_callback;
    const size_t m_expires_seconds;
    size_t m_announce_count;
};

} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__SSDP__SSDP_ADVERT_H__
