/**
 * @file
 *
 * Implementation for the bbox::rt::net::SsdpSearch class.
 */

#include <bbox/rt/net/ssdp/SsdpSearch.h>
#include <bbox/rt/net/ssdp/SsdpDiscoveryService.h>

#include <bbox/ScopedDebugIndent.h>
#include <bbox/enc/ToDebugOutput.h>

namespace bbox {
namespace rt {
namespace net {
namespace ssdp {

SsdpSearch::SsdpSearch(const std::string &name, Service &parent, std::string &&service_type)
    : Service(name, parent)
    , m_discovery_service_ref("discovery-service-ref", *this, SsdpDiscoveryService::SERVICE_NAME)
    , m_timer("timer", *this, std::bind(&SsdpSearch::HandleTimerExpired, this))
    , m_change_work("change-work", *this, std::bind(&SsdpSearch::HandleDevicesChanged, this))
    , m_uuid(Uuid::NewRandom())
    , m_service_type(std::move(service_type))
    , m_search_duration(60)
    , m_announce_count(0)
    , m_search_counter(0)
{
}

SsdpSearch::~SsdpSearch()
{
}

void SsdpSearch::HandleStarting()
{
    auto insert_result = m_discovery_service_ref->m_search_ptrs.insert(this);
    BBOX_ASSERT(insert_result.second);

    m_devices.clear();
    m_announce_count = 0;
    m_timer.StartPeriodic(TimeSpan::Seconds(1));

    NotifyStarted();
}

void SsdpSearch::HandleStopping()
{
    auto erase_result = m_discovery_service_ref->m_search_ptrs.erase(this);
    BBOX_ASSERT(erase_result == 1);

    m_devices.clear();

    m_timer.Cancel();
    m_change_work.Cancel();

    RequestStopAllChildren();
    NotifyStopped();
}

void SsdpSearch::PrintState(bbox::DebugOutput &out) const
{
    out.Format("Search Type:    %s\n", m_service_type);
    out.Format("UUID:           %s\n", m_uuid.ToUrn());
    out.Format("Announce Count: %d\n", m_announce_count);
    out.Format("Search Counter: %d\n", m_search_counter);
    out.Format("Devices:\n");
    bbox::ScopedDebugIndent(out, 4);
    bbox::enc::ToDebugOutput(out, m_devices);
}

void SsdpSearch::RestartAdvertTimeOnNetworkChange()
{
    if (GetLocalRunLevel() == RunLevel::RUNNING)
    {
        m_announce_count = 0;
        m_timer.StartPeriodic(TimeSpan::Seconds(1));
    }
}

void SsdpSearch::HandleTimerExpired()
{
    m_announce_count += 1;

    m_discovery_service_ref->SendSearches(this, m_search_counter);

    if (m_announce_count > 3)
    {
        m_search_counter += 1;
        m_timer.StartSingleShot(TimeSpan::Seconds(m_search_duration));
    }
}

void SsdpSearch::HandleDevicesChanged()
{
    // TODO - notify user
}

void SsdpSearch::RemoveDevice(const std::string &usn)
{
    auto erase_result = m_devices.erase(usn);

    if (erase_result != 0)
        m_change_work.Schedule();
}

void SsdpSearch::RemoveInterface(const IpAddress &local_addr)
{
    std::set<std::string> to_erase;

    for (const auto &entry : m_devices)
    {
        if (entry.second.local_address == local_addr)
        {
            to_erase.insert(entry.first);
        }
    }

    if (!to_erase.empty())
    {
        for (const auto &entry : to_erase)
            m_devices.erase(entry);

        m_change_work.Schedule();
    }
}

void SsdpSearch::HandleNotification(const IpAddress &local_addr, const IpAddress &device_addr, const std::string &usn, const std::string &search_type, const std::string &location)
{
    DeviceInfo info;

    info.local_address = local_addr;
    info.device_address = device_addr;
    info.usn = usn;
    info.service_type = search_type;
    info.location = location;

    auto it = m_devices.find(usn);

    if (it == m_devices.end())
    {
        m_devices[usn] = std::move(info);
        m_change_work.Schedule();
    }
    else if (info != it->second)
    {
        it->second = std::move(info);
        m_change_work.Schedule();
    }
}
 
} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
