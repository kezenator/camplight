/**
 * @file
 *
 * Header for the bbox::rt::net::SsdpDiscoveryService class.
 */

#ifndef __BBOX__RT__NET__SSDP__SSDP_DISCOVERY_SERVICE_H__
#define __BBOX__RT__NET__SSDP__SSDP_DISCOVERY_SERVICE_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/net/NetworkChangeHandler.h>
#include <bbox/rt/OneShotWork.h>
#include <bbox/rt/net/UdpEndpoint.h>
#include <bbox/Error.h>

namespace bbox {
namespace rt {
namespace net {
namespace ssdp {

/**
 * A class that advertises a service via Upnp, and discovers
 * other services.
 */
class SsdpDiscoveryService : public Service
{
public:
    SsdpDiscoveryService(const std::string &name, Service &parent);
    ~SsdpDiscoveryService();

private:
    void HandleStarting() override;
    void HandleStopping() override;
    void PrintState(bbox::DebugOutput &out) const override;

    void OnNetworkInterfacesChanged();
    void OnCheckNetwork();

    struct NetworkInterface;

    void HandleReceivedPacket(NetworkInterface *iface_ptr, const Error &err, const void *data, size_t num_bytes, const UdpEndpoint &from);

    NetworkChangeHandler m_network_change_handler;
    OneShotWork m_check_network_work;
    std::vector<std::unique_ptr<NetworkInterface>> m_interfaces;
};

} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__SSDP__SSDP_DISCOVERY_SERVICE_H__
