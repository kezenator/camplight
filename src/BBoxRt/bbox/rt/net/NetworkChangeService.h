/**
 * @file
 *
 * Header for the bbox::rt::net::NetworkChangeService class.
 */

#ifndef __BBOX__RT__NET__NETWORK_CHANGE_SERVICE_H__
#define __BBOX__RT__NET__NETWORK_CHANGE_SERVICE_H__

#pragma once

#include <map>
#include <bbox/rt/Service.h>
#include <bbox/rt/net/AdapterInfo.h>

namespace bbox {
namespace rt {

#ifdef WIN32
namespace win32 {
// Forward declaration
class Win32NetworkChangeService;
} // namespace bbox::rt::win32
#else // not WIN32
namespace lin {
// Forward declaration
class LinuxNetworkChangeService;
} // namespace bbox::rt::win32
#endif // not WIN32

namespace net {

// Forward declaration
class NetworkChangeHandler;

/**
 * A service that detects networks adapters connected to the PC.
 */
class NetworkChangeService : public Service
{
	friend class NetworkChangeHandler;

public:

	static const char *SERVICE_NAME;

#ifdef WIN32
	using Impl = win32::Win32NetworkChangeService;
#else // not WIN32
	using Impl = lin::LinuxNetworkChangeService;
#endif // not WIN32

	const std::map<std::string, net::AdapterInfo> &GetCurrentAdapterInfo() const
	{
		return m_current_adapters;
	}

protected:

	NetworkChangeService(const std::string &name, Proactor &parent);
	NetworkChangeService(const std::string &name, Service &parent);
	virtual ~NetworkChangeService();

	void ReportChange(std::map<std::string, net::AdapterInfo> &&new_adapters);

private:
	std::map<std::string, net::AdapterInfo> m_current_adapters;
	std::set<NetworkChangeHandler *, std::less<NetworkChangeHandler *>> m_handlers;
};

} // namespae bbox::rt::net
} // namespace bbox::rt
} // namespace bbox

#ifdef WIN32
#include <bbox/rt/win32/Win32NetworkChangeService.h>
#else // not WIN32
#include <bbox/rt/lin/LinuxNetworkChangeService.h>
#endif // not WIN32

#endif // __BBOX__RT__NET__NETWORK_CHANGE_SERVICE_H__
