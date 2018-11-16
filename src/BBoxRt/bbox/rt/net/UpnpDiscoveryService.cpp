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
        m_parent.HandleReceivedPacket(this, err, data, num_bytes, from);
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
    std::vector<IpAddress> to_destroy;

    // If not stopping, then we should create any
    // interfaces that have been found but that
    // we don't have

    if (GetLocalRunLevel() == RunLevel::RUNNING)
    {
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
    }

    // We should check all created interfaces and destroy
    // any that don't exist, or all if shutting down

    for (const auto &iface_ptr : m_interfaces)
    {
        bool found = false;

        if (GetLocalRunLevel() != RunLevel::STOPPING)
        {
            for (const IpAddress &addr : cur_addresses)
            {
                if (iface_ptr->m_address == addr)
                {
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            to_destroy.push_back(iface_ptr->m_address);
        }
    }

    bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Never);
    if (out)
        out.Format("UpnpDiscoveryService::OnCheckNetwork\n");

    // Create new interfaces

    for (const IpAddress &addr : to_create)
    {
        if (out)
            out.Format("    Creating %s\n", addr.ToString());

        m_interfaces.emplace_back(std::make_unique<NetworkInterface>(addr.ToString(), *this, addr));
    }

    // Request destroy of interfaces

    for (const IpAddress &addr : to_destroy)
    {
        for (const auto &iface_ptr : m_interfaces)
        {
            if ((iface_ptr->m_address == addr)
                && (iface_ptr->GetOverallRunLevel() == RunLevel::RUNNING))
            {
                if (out)
                    out.Format("    Stopping %s\n", iface_ptr->m_address.ToString());

                iface_ptr->RequestStop([this]()
                    {
                        m_check_network_work.Schedule();
                    });
            }
        }
    }

    // Destroy interfaces that we can

    while (true)
    {
        bool destroyed_any = false;

        for (size_t i = 0; i < m_interfaces.size(); ++i)
        {
            if (m_interfaces[i]->GetOverallRunLevel() == RunLevel::STOPPED)
            {
                if (out)
                    out.Format("    Destroying %s\n", m_interfaces[i]->m_address.ToString());

                destroyed_any = true;
                m_interfaces.erase(m_interfaces.begin() + i);
                m_check_network_work.Schedule();
            }
        }

        if (!destroyed_any)
            break;
    }

    // Shutdown

    if ((GetLocalRunLevel() == RunLevel::STOPPING)
        && m_interfaces.empty())
    {
        m_check_network_work.Cancel();

        RequestStopAllChildren();
        NotifyStopped();
    }
}

void UpnpDiscoveryService::HandleReceivedPacket(NetworkInterface *iface_ptr, const Error &err, const void *data, size_t num_bytes, const UdpEndpoint &from)
{
    bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
    if (out)
    {
        out.Format("SSDP Rx: iface=%s, err=%s, num_bytes=%d, from=%s\n",
            iface_ptr->m_address.ToString(),
            err.ToString(),
            num_bytes,
            from.ToString());

        if (!err)
            out.PrintData(data, num_bytes);
    }
}

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
