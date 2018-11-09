/**
 * @file
 *
 * Header for the bbox::rt::net::UpnpDiscoveryService class.
 */

#ifndef __BBOX__RT__NET__UPNP_DISCOVERY_SERVICE_H__
#define __BBOX__RT__NET__UPNP_DISCOVERY_SERVICE_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/net/NetworkChangeHandler.h>
#include <bbox/rt/OneShotWork.h>

namespace bbox {
namespace rt {
namespace net {

/**
 * A class that advertises a service via Upnp, and discovers
 * other services.
 */
class UpnpDiscoveryService: public Service
{
public:
	UpnpDiscoveryService(const std::string &name, Service &parent);
	~UpnpDiscoveryService();

private:
	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	void OnNetworkInterfacesChanged();
	void OnCheckNetwork();

	struct NetworkInterface;

	NetworkChangeHandler m_network_change_handler;
	OneShotWork m_check_network_work;
	std::vector<std::unique_ptr<NetworkInterface>> m_interfaces;
};

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__UPNP_DISCOVERY_SERVICE_H__
