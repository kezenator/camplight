/**
 * @file
 *
 * Implementation for the bbox::rt::net::SsdpDiscoveryService class.
 */

#include <bbox/rt/net/ssdp/SsdpDiscoveryService.h>
#include <bbox/rt/net/ssdp/SsdpAdvert.h>
#include <bbox/rt/net/ssdp/SsdpSearch.h>
#include <bbox/rt/net/UdpSocket.h>

#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace bbox {
namespace rt {
namespace net {
namespace ssdp {

struct SsdpDiscoveryService::NetworkInterface : Resource
{
    NetworkInterface(const std::string &name, SsdpDiscoveryService &parent, const IpAddress &addr)
        : Resource(name, parent)
        , m_parent(parent)
        , m_address(addr)
        , m_socket("socket", *this)
    {
        BBOX_ASSERT(m_address.is_v4());

        m_socket.Open(boost::asio::ip::udp::v4());
        m_socket.SetOption(boost::asio::ip::udp::socket::reuse_address(true));
        m_socket.Bind(UdpEndpoint(m_address, m_parent.m_ssdp_multicast_endpoint.GetPort()));
        m_socket.SetOption(boost::asio::ip::multicast::join_group(m_parent.m_ssdp_multicast_endpoint.address().to_v4(), m_address.to_v4()));
        m_socket.EnableReading(std::bind(&NetworkInterface::OnSocketReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    SsdpDiscoveryService &m_parent;
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

SsdpDiscoveryService::SsdpDiscoveryService(const std::string &name, Service &parent)
    : Service(name, parent)
    , m_network_change_handler("network-change-handler", *this, std::bind(&SsdpDiscoveryService::OnNetworkInterfacesChanged, this))
    , m_check_network_work("check-network-work", *this, std::bind(&SsdpDiscoveryService::OnCheckNetwork, this))
    , m_interfaces()
    , m_advert_ptrs()
    , m_ssdp_multicast_endpoint()
    , m_data_debug("data-debug", *this)
    , m_event_debug("event-debug", *this)
{
    RegisterService(SERVICE_NAME, this);

    IpAddress multicast;
    IpAddress::FromString("239.255.255.250", multicast);
    m_ssdp_multicast_endpoint = UdpEndpoint(multicast, 1900);
}

SsdpDiscoveryService::~SsdpDiscoveryService()
{
}

void SsdpDiscoveryService::HandleStarting()
{
    m_check_network_work.Schedule();

    NotifyStarted();
}

void SsdpDiscoveryService::HandleStopping()
{
    m_check_network_work.Schedule();
}

void SsdpDiscoveryService::PrintState(bbox::DebugOutput &out) const
{
    // TODO
    (void)out;
}

void SsdpDiscoveryService::OnNetworkInterfacesChanged()
{
    if (GetLocalRunLevel() == RunLevel::RUNNING)
    {
        m_check_network_work.Schedule();
    }
}

void SsdpDiscoveryService::OnCheckNetwork()
{
    // Work out what interfaces to create and destroy

    std::vector<IpAddress> cur_addresses;

    for (const auto &entry : m_network_change_handler.GetCurrentAdapterInfo())
    {
        for (const auto &addr_info : entry.second.ip_addresses)
        {
            if (addr_info.address.is_v4()
                && !addr_info.address.is_loopback())
            {
                cur_addresses.push_back(addr_info.address);
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

    bbox::DebugOutput out(BBOX_FUNC, m_event_debug);
    if (out)
        out.Format("SsdpDiscoveryService::OnCheckNetwork\n");

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

                for (SsdpSearch *search_ptr : m_search_ptrs)
                {
                    search_ptr->RemoveInterface(m_interfaces[i]->m_address);
                }

                destroyed_any = true;
                m_interfaces.erase(m_interfaces.begin() + i);
                m_check_network_work.Schedule();
            }
        }

        if (!destroyed_any)
            break;
    }

    // Restart the adverts if there were any new interfaces created

    if (!to_create.empty())
    {
        for (SsdpAdvert *advert_ptr : m_advert_ptrs)
        {
            advert_ptr->RestartAdvertTimeOnNetworkChange();
        }
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

void SsdpDiscoveryService::SendNotifies(SsdpAdvert *advert_ptr, bool alive)
{
    for (const auto &iface_ptr : m_interfaces)
    {
        SendNotify(iface_ptr.get(), advert_ptr, alive);
    }
}

void SsdpDiscoveryService::SendSearches(SsdpSearch *search_ptr, size_t count)
{
    for (const auto &iface_ptr : m_interfaces)
    {
        SendSearch(iface_ptr.get(), search_ptr, count);
    }
}

void SsdpDiscoveryService::SendNotify(NetworkInterface *iface_ptr, SsdpAdvert *advert_ptr, bool alive)
{
    bool should_send = false;
    std::string location;

    advert_ptr->m_info_callback(iface_ptr->m_address, should_send, location);

    if (should_send)
    {
        DebugOutput event_out(BBOX_FUNC, m_event_debug);
        if (event_out)
        {
            event_out.Format("SSDP Notify: iface=%s, search_type=%s, advert=",
                iface_ptr->m_address.ToString(), advert_ptr->m_service_type);
            advert_ptr->PrintResourcePathLink(event_out);
            event_out.Format(", alive=%s\n", alive);
        }

        std::stringstream stream;

        stream << "NOTIFY * HTTP/1.1\r\n";
        stream << "Host: " << m_ssdp_multicast_endpoint.ToString() << "\r\n";
        stream << "NT: " << advert_ptr->m_service_type << "\r\n";
        stream << "NTS: " << (alive ? "ssdp:alive" : "ssdp:byebye") << "\r\n";
        stream << "USN: " << advert_ptr->m_uuid.ToUrn() << "\r\n";
        stream << "Location: " << location << "\r\n";

        if (alive)
            stream << "Cache-Control: max-age=" << advert_ptr->m_expires_seconds << "\r\n";

        stream << "\r\n";

        SendPacket(iface_ptr, m_ssdp_multicast_endpoint, stream.str());
    }
}

void SsdpDiscoveryService::SendSearch(NetworkInterface *iface_ptr, SsdpSearch *search_ptr, size_t count)
{
    DebugOutput event_out(BBOX_FUNC, m_event_debug);
    if (event_out)
    {
        event_out.Format("SSDP Search: iface=%s, search_type=%s, search=",
            iface_ptr->m_address.ToString(), search_ptr->m_service_type);
        search_ptr->PrintResourcePathLink(event_out);
        event_out.Format("\n");
    }

    std::stringstream stream;

    stream << "M-SEARCH * HTTP/1.1\r\n";
    stream << "Host: " << m_ssdp_multicast_endpoint.ToString() << "\r\n";
    stream << "S: " << search_ptr->m_uuid.ToUrn() << ":" << count << "\r\n";
    stream << "Man: \"ssdp:discover\"\r\n";
    stream << "ST: " << search_ptr->m_service_type << "\r\n";
    stream << "MX: 3\r\n";
    stream << "\r\n";

    SendPacket(iface_ptr, m_ssdp_multicast_endpoint, stream.str());
}

void SsdpDiscoveryService::SendSearchResponse(NetworkInterface *iface_ptr, const UdpEndpoint &dest, const std::string &search_id, SsdpAdvert *advert_ptr)
{
    bool should_send = false;
    std::string location;

    advert_ptr->m_info_callback(iface_ptr->m_address, should_send, location);

    if (should_send)
    {
        std::stringstream stream;

        stream << "HTTP/1.1 200 OK\r\n";
        if (!search_id.empty())
            stream << "S: " << search_id << "\r\n";
        stream << "Ext:\r\n";
        stream << "Cache-Control: max-age=" << advert_ptr->m_expires_seconds << "\r\n";
        stream << "ST: " << advert_ptr->m_service_type << "\r\n";
        stream << "USN: " << advert_ptr->m_uuid.ToUrn() << "\r\n";
        stream << "Location: " << location << "\r\n";
        stream << "\r\n";

        SendPacket(iface_ptr, dest, stream.str());
    }
}

void SsdpDiscoveryService::SendPacket(NetworkInterface *iface_ptr, const UdpEndpoint &dest, const std::string &packet)
{
    bbox::DebugOutput out(BBOX_FUNC, m_data_debug);

    bbox::Error err = iface_ptr->m_socket.SendTo(
        packet.c_str(),
        packet.size(),
        dest);

    if (out)
    {
        out.Format("SSDP Tx: iface=%s, err=%s, num_bytes=%d, to=%s\n",
            iface_ptr->m_address.ToString(),
            err.ToString(),
            packet.size(),
            dest.ToString());

        if (!err)
            out.PrintData(packet.c_str(), packet.size());
    }
}

void SsdpDiscoveryService::HandleReceivedPacket(NetworkInterface *iface_ptr, const Error &err, const void *data, size_t num_bytes, const UdpEndpoint &from)
{
    bbox::DebugOutput data_out(BBOX_FUNC, m_data_debug);
    if (data_out)
    {
        data_out.Format("SSDP Rx: iface=%s, err=%s, num_bytes=%d, from=%s\n",
            iface_ptr->m_address.ToString(),
            err.ToString(),
            num_bytes,
            from.ToString());

        if (!err)
            data_out.PrintData(data, num_bytes);
    }

    // Decode the message

    if ((num_bytes > 9)
        && ((0 == memcmp(data, "M-SEARCH ", 9))
            || (0 == memcmp(data, "NOTIFY ", 7))))
    {
        // It look like an M-SEARCH or NOTIFY - which should be coded as
        // HTTP requests

        boost::system::error_code ec;
        boost::beast::http::request_parser<boost::beast::http::string_body> p;
        p.put(boost::asio::buffer(data, num_bytes), ec);

        if (!ec)
        {
            boost::beast::http::request<boost::beast::http::string_body> request = p.release();

            if ((request.method() == boost::beast::http::verb::msearch)
                && (request.find("ST") != request.end())
                && (request.find("Man") != request.end()))
            {
                std::string man = request.find("Man")->value().to_string();

                if (man == "\"ssdp:discover\"")
                {
                    std::string search_type = request.find("ST")->value().to_string();
                    std::string search_id;

                    if (request.find("S") != request.end())
                    {
                        search_id = request.find("S")->value().to_string();
                    }

                    for (SsdpAdvert *advert_ptr : m_advert_ptrs)
                    {
                        if ((search_type == "ssdp:all")
                            || (search_type == advert_ptr->m_service_type))
                        {
                            DebugOutput event_out(BBOX_FUNC, m_event_debug);
                            if (event_out)
                            {
                                event_out.Format("SSDP Discover: %s => %s, search_type=%s, advert=",
                                    from.ToString(), iface_ptr->m_address.ToString(), search_type);
                                advert_ptr->PrintResourcePathLink(event_out);
                                event_out.Format("\n");
                            }

                            SendSearchResponse(iface_ptr, from, search_id, advert_ptr);
                        }
                    }
                }
            }
            else if ((request.method() == boost::beast::http::verb::notify)
                && (request.find("NT") != request.end())
                && (request.find("NTS") != request.end())
                && (request.find("USN") != request.end()))
            {
                std::string search_type = request.find("NT")->value().to_string();
                std::string alive = request.find("NTS")->value().to_string();
                std::string usn = request.find("USN")->value().to_string();
                std::string location;

                if (request.find("Location") != request.end())
                {
                    location = request.find("Location")->value().to_string();
                }

                if (alive == "ssdp:byebye")
                {
                    for (SsdpSearch *search_ptr : m_search_ptrs)
                    {
                        if ((search_type == search_ptr->m_service_type)
                            || (search_ptr->m_service_type == "ssdp:all"))
                        {
                            DebugOutput event_out(BBOX_FUNC, m_event_debug);
                            if (event_out)
                            {
                                event_out.Format("SSDP ByeBye: %s => %s, usn=%s, search_type=%s, search=",
                                    from.ToString(), iface_ptr->m_address.ToString(), usn, search_type);
                                search_ptr->PrintResourcePathLink(event_out);
                                event_out.Format("\n");
                            }

                            search_ptr->RemoveDevice(usn);
                        }
                    }
                }
                else if (alive == "ssdp:alive")
                {
                    for (SsdpSearch *search_ptr : m_search_ptrs)
                    {
                        if ((search_type == search_ptr->m_service_type)
                            || (search_ptr->m_service_type == "ssdp:all"))
                        {
                            DebugOutput event_out(BBOX_FUNC, m_event_debug);
                            if (event_out)
                            {
                                event_out.Format("SSDP Alive: %s => %s, usn=%s, search_type=%s, location=%s, search=",
                                    from.ToString(), iface_ptr->m_address.ToString(), usn, search_type, location);
                                search_ptr->PrintResourcePathLink(event_out);
                                event_out.Format("\n");
                            }

                            search_ptr->HandleNotification(iface_ptr->m_address, from.GetAddress(), usn, search_type, location);
                        }
                    }
                }
            }
        }
    }
    else if ((num_bytes > 9)
        && (0 == memcmp(data, "HTTP/1.1 ", 9)))
    {
        // It looks like a HTTP response

        boost::system::error_code ec;
        boost::beast::http::response_parser<boost::beast::http::string_body> p;
        p.put(boost::asio::buffer(data, num_bytes), ec);

        if (!ec)
        {
            boost::beast::http::response<boost::beast::http::string_body> response = p.release();

            if ((response.result() == boost::beast::http::status::ok)
                && (response.find("ST") != response.end())
                && (response.find("USN") != response.end()))
            {
                std::string search_type = response.find("ST")->value().to_string();
                std::string usn = response.find("USN")->value().to_string();
                std::string location;

                if (response.find("Location") != response.end())
                {
                    location = response.find("Location")->value().to_string();
                }

                for (SsdpSearch *search_ptr : m_search_ptrs)
                {
                    if ((search_type == search_ptr->m_service_type)
                        || (search_ptr->m_service_type == "ssdp:all"))
                    {
                        DebugOutput event_out(BBOX_FUNC, m_event_debug);
                        if (event_out)
                        {
                            event_out.Format("SSDP Response: %s => %s, usn=%s, search_type=%s, location=%s, search=",
                                from.ToString(), iface_ptr->m_address.ToString(), usn, search_type, location);
                            search_ptr->PrintResourcePathLink(event_out);
                            event_out.Format("\n");
                        }

                        search_ptr->HandleNotification(iface_ptr->m_address, from.GetAddress(), usn, search_type, location);
                    }
                }
            }
        }
    }
}

} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
