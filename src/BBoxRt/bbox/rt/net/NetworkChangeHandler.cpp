/**
* @file
*
* Implementation for the bbox::rt::net::NetworkChangeHandler class.
*/

#include <bbox/rt/net/NetworkChangeHandler.h>
#include <bbox/Assert.h>

namespace bbox {
namespace rt {
namespace net {

NetworkChangeHandler::NetworkChangeHandler(const std::string &name, Service &parent, std::function<void()> &&change_handler)
	: Service(name, parent)
	, m_service_ref("service-reference", *this, NetworkChangeService::SERVICE_NAME)
	, m_change_handler(std::move(change_handler))
{
}

NetworkChangeHandler::~NetworkChangeHandler()
{
}

std::map<std::string, net::AdapterInfo> NetworkChangeHandler::GetCurrentAdapterInfo()
{
	BBOX_ASSERT((GetLocalRunLevel() == RunLevel::RUNNING) || (GetLocalRunLevel() == RunLevel::STOPPING));

	return m_service_ref->GetCurrentAdapterInfo();
}

void NetworkChangeHandler::HandleStarting()
{
	auto insert_result = m_service_ref->m_handlers.insert(this);
	BBOX_ASSERT(insert_result.second == true);
	
	NotifyStarted();
}

void NetworkChangeHandler::HandleStopping()
{
	auto erase_result = m_service_ref->m_handlers.erase(this);
	BBOX_ASSERT(erase_result == 1);

	RequestStopAllChildren();
	NotifyStopped();
}

void NetworkChangeHandler::PrintState(bbox::DebugOutput &out) const
{
}

} // namespace bbox::rt::net
} // namespace bbox::rt
} // namespace bbox
