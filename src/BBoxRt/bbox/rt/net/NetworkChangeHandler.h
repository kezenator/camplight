/**
 * @file
 *
 * Header for the bbox::rt::net::NetworkChangeHandler class.
 */

#ifndef __BBOX__RT__NET__NETWORK_CHANGE_HANDLER_H__
#define __BBOX__RT__NET__NETWORK_CHANGE_HANDLER_H__

#pragma once

#include <bbox/rt/ServiceReference.h>
#include <bbox/rt/net/NetworkChangeService.h>

namespace bbox {
namespace rt {
namespace net {

/**
 * References the network change service
 * to get changes to network interfaces.
 */
class NetworkChangeHandler : public Service
{
    friend class NetworkChangeService;

public:
	NetworkChangeHandler(const std::string &name, Service &parent, std::function<void()> &&change_handler);
	~NetworkChangeHandler();

	std::map<std::string, net::AdapterInfo> GetCurrentAdapterInfo();

private:

	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	ServiceReference<NetworkChangeService> m_service_ref;
	std::function<void()> m_change_handler;
};

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__NET__NETWORK_CHANGE_HANDLER_H__