/**
 * @file
 *
 * Implementation for the bbox::rt::net::UpnpDiscoveryService class.
 */

#include <bbox/rt/net/UpnpDiscoveryService.h>
#include <bbox/rt/net/UdpSocket.h>

namespace bbox {
namespace rt {
namespace net {

struct UpnpDiscoveryService::NetworkInterface: Resource
{
	NetworkInterface(const std::string &name, UpnpDiscoveryService &parent, const IpAddress &addr)
		: Resource(name, parent)
		, m_parent(parent)
		, m_address(addr)
		, m_socket("socket", *this)
	{
		BBOX_ASSERT(m_address.is_v4());

		IpAddress multicast;
		IpAddress::FromString("239.255.255.250", multicast);
		m_socket.Open(boost::asio::ip::udp::v4());
		m_socket.SetOption(boost::asio::ip::udp::socket::reuse_address(true));
		m_socket.Bind(UdpEndpoint(m_address, 1900));
		m_socket.SetOption(boost::asio::ip::multicast::join_group(multicast.to_v4(), m_address.to_v4()));
		m_socket.EnableReading(std::bind(&NetworkInterface::OnSocketReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}

	UpnpDiscoveryService &m_parent;
	const IpAddress m_address;
	UdpSocket m_socket;

	void HandleStopping() override
	{
		RequestStopAllChildren();
		NotifyStopped();
	}

	void PrintState(bbox::DebugOutput &out) const override
	{
		out.Format("Address: %s\n", m_address.ToString());
	}

	void OnSocketReceive(const Error &err, const void *data, size_t num_bytes, const UdpEndpoint &from)
	{
	}
};

UpnpDiscoveryService::UpnpDiscoveryService(const std::string &name, Service &parent)
	: Service(name, parent)
	, m_network_change_handler("network-change-handler", *this, std::bind(&UpnpDiscoveryService::OnNetworkInterfacesChanged, this))
	, m_check_network_work("check-network-work", *this, std::bind(&UpnpDiscoveryService::OnCheckNetwork, this))
	, m_interfaces()
{
}

UpnpDiscoveryService::~UpnpDiscoveryService()
{
}

void UpnpDiscoveryService::HandleStarting()
{
	m_check_network_work.Schedule();

	NotifyStarted();
}

void UpnpDiscoveryService::HandleStopping()
{
	m_check_network_work.Schedule();
}

void UpnpDiscoveryService::PrintState(bbox::DebugOutput &out) const
{
	// TODO
	(void)out;
}

void UpnpDiscoveryService::OnNetworkInterfacesChanged()
{
	if (GetLocalRunLevel() == RunLevel::RUNNING)
	{
		m_check_network_work.Schedule();
	}
}

void UpnpDiscoveryService::OnCheckNetwork()
{
	// Work out what interfaces to create and destroy

	std::vector<IpAddress> cur_addresses;

	for (const auto &entry : m_network_change_handler.GetCurrentAdapterInfo())
	{
		for (const std::string &addr_str : entry.second.ip_addresses)
		{
			IpAddress addr;
			if (IpAddress::FromString(addr_str, addr)
				&& addr.is_v4()
				&& !addr.is_loopback())
			{
				cur_addresses.push_back(addr);
			}
		}
	}

	std::vector<IpAddress> to_create;
	std::vector<size_t> to_destroy;

	for (const IpAddress &addr : cur_addresses)
	{
		bool found = false;

		for (const auto &iface_ptr : m_interfaces)
		{
			if (iface_ptr->m_address == addr)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			to_create.push_back(addr);
		}
	}

	// Create new interfaces

	for (const IpAddress &addr : to_create)
	{
		m_interfaces.emplace_back(std::make_unique<NetworkInterface>(addr.ToString(), *this, addr));
	}

	// Destroy interfaces

	// Shutdown

	if ((GetLocalRunLevel() == RunLevel::STOPPING)
		&& m_interfaces.empty())
	{
		RequestStopAllChildren();
		NotifyStopped();
	}
}

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
