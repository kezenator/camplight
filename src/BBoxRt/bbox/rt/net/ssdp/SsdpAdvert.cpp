/**
 * @file
 *
 * Implementation for the bbox::rt::net::SsdpAdvert class.
 */

#include <bbox/rt/net/ssdp/SsdpAdvert.h>
#include <bbox/rt/net/ssdp/SsdpDiscoveryService.h>

namespace bbox {
namespace rt {
namespace net {
namespace ssdp {

SsdpAdvert::SsdpAdvert(const std::string &name, Service &parent, std::string &&service_type, InfoCallback &&info_callback)
    : Service(name, parent)
    , m_discovery_service_ref("discovery-service-ref", *this, SsdpDiscoveryService::SERVICE_NAME)
    , m_timer("timer", *this, std::bind(&SsdpAdvert::HandleTimerExpired, this))
    , m_uuid(Uuid::NewRandom())
    , m_service_type(std::move(service_type))
    , m_info_callback(std::move(info_callback))
    , m_expires_seconds(3600)
    , m_announce_count(0)
{
}

SsdpAdvert::~SsdpAdvert()
{
}

void SsdpAdvert::HandleStarting()
{
    auto insert_result = m_discovery_service_ref->m_advert_ptrs.insert(this);
    BBOX_ASSERT(insert_result.second);

    m_announce_count = 0;
    m_timer.StartPeriodic(TimeSpan::Seconds(1));

    NotifyStarted();
}

void SsdpAdvert::HandleStopping()
{
    auto erase_result = m_discovery_service_ref->m_advert_ptrs.erase(this);
    BBOX_ASSERT(erase_result == 1);

    m_announce_count = 0;
    m_timer.StartPeriodic(TimeSpan::Seconds(1));
}

void SsdpAdvert::PrintState(bbox::DebugOutput &out) const
{
    // TODO
    (void)out;
}

void SsdpAdvert::RestartAdvertTimeOnNetworkChange()
{
    if (GetLocalRunLevel() == RunLevel::RUNNING)
    {
        m_announce_count = 0;
        m_timer.StartPeriodic(TimeSpan::Seconds(1));
    }
}

void SsdpAdvert::HandleTimerExpired()
{
    m_announce_count += 1;

    m_discovery_service_ref->SendNotifies(this, GetLocalRunLevel() == RunLevel::RUNNING);

    if (m_announce_count > 3)
    {
        if (GetLocalRunLevel() == RunLevel::STOPPING)
        {
            m_timer.Cancel();

            RequestStopAllChildren();
            NotifyStopped();
        }
        else
        {
            m_timer.StartSingleShot(TimeSpan::Seconds(m_expires_seconds / 3));
        }
    }
}

} // namespace bbox::rt::net::ssdp
} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
