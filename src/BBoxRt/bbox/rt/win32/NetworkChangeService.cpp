/**
* @file
*
* Implementation for the bbox::rt::win32::NetworkChangeService class.
*/

#include <bbox/rt/win32/NetworkChangeService.h>
#include <bbox/rt/win32/IpHelperAccess.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>

#include <boost/bind.hpp>

namespace {

    void Callback(void *context)
    {
        bbox::rt::win32::NetworkChangeService *service = static_cast<bbox::rt::win32::NetworkChangeService *>(context);

        service->TriggerUpdate();
    }

} // annonymous namespace

namespace bbox {
    namespace rt {
        namespace win32 {

            const char *NetworkChangeService::SERVICE_NAME = "win32/NetworkChangeService";

            NetworkChangeService::NetworkChangeService(const std::string &name, Proactor &parent)
                : Service(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_change_handle(nullptr)
                , m_pending_counter(0)
                , m_another_update_required(false)
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::NetworkChangeService(const std::string &name, Service &parent)
                : Service(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_change_handle(nullptr)
                , m_pending_counter(0)
                , m_another_update_required(false)
            {
                RegisterService(SERVICE_NAME, this);
            }

            NetworkChangeService::~NetworkChangeService()
            {
            }

            void NetworkChangeService::HandleStarting()
            {
                // Register for change notifications

                IpHelperAccess::RegisterChangeNotifier(
                    m_change_handle,
                    &Callback,
                    this);
            }

            void NetworkChangeService::HandleStopping()
            {
                // To start the shutdown process, we want to
                // destroy the change notifier
                // and start shutting down the thread pool access

                IpHelperAccess::DestroyChangeNotifier(m_change_handle);
                m_thread_pool_ref.RequestStop(boost::bind(&NetworkChangeService::CheckShutdown, this));

                CheckShutdown();
            }

            void NetworkChangeService::CheckShutdown()
            {
                if ((GetLocalRunLevel() == RunLevel::STOPPING)
                    && (m_thread_pool_ref.GetOverallRunLevel() == RunLevel::STOPPED)
                    && (0 == m_pending_counter))
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

			void NetworkChangeService::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Current adapters: %d\n", m_current_adapters.size());

				ScopedDebugIndent indent(out, 4);

				for (const auto &entry : m_current_adapters)
				{
					out.Format("\n");
					out.Format("Name: %s\n", entry.first);
					out.Format("System Name: %s\n", entry.second.system_name);
					out.Format("User Name: %s\n", entry.second.user_name);
					out.Format("Description: %s\n", entry.second.description);
					out.Format("MAC Address: %s\n", entry.second.mac_address);

					for (const std::string &addr : entry.second.ip_addresses)
						out.Format("IP Address: %s\n", addr);
				}
			}

            void NetworkChangeService::TriggerUpdate()
            {
                // This function is called from a random
                // thread created by the IpHelper API.

                // Increment the pending count
                // and thread-safe post a function to be run
                // back in the main thread.

                m_pending_counter.fetch_add(1, boost::memory_order_relaxed);

                GetProactor().ThreadSafePost(boost::bind(
                    &NetworkChangeService::HandleUpdateRequired,
                    this));
            }

            void NetworkChangeService::HandleUpdateRequired()
            {
                // Decrement the pending operation count

                m_pending_counter.fetch_sub(1, boost::memory_order_release);

                // There there's no thread pool operation running,
                // then start one - otherwise mark another update
                // as required

                if (!m_thread_pool_ref.AnyOperationsRunning())
                {
                    m_thread_pool_ref.RunOnThread(
                        boost::bind(&NetworkChangeService::RequestAdapters, this),
                        boost::bind(&NetworkChangeService::HandlUpdateComplete, this));
                }
                else
                {
                    m_another_update_required = true;
                }
            }

            void NetworkChangeService::HandlUpdateComplete()
            {
                BBOX_ASSERT(!m_thread_pool_ref.AnyOperationsRunning());

                // If we're shutting down then ignore.
                // If there's another update requred, then
                // start it. Otherwise - handle this change.

                if (GetLocalRunLevel() == RunLevel::STOPPING)
                {
                    CheckShutdown();
                }
                else if (m_another_update_required)
                {
                    m_another_update_required = false;

                    m_thread_pool_ref.RunOnThread(
                        boost::bind(&NetworkChangeService::RequestAdapters, this),
                        boost::bind(&NetworkChangeService::HandlUpdateComplete, this));
                }
                else
                {
                    if ((m_detecting_adapters != m_current_adapters)
                        || (GetLocalRunLevel() == RunLevel::STARTING))
                    {
                        // We've got new settings - save them, and either
                        // mark us as started or post a change notification

                        m_current_adapters = std::move(m_detecting_adapters);

                        {
                            std::cout << "Network Adapters - "
                                << ((GetLocalRunLevel() == RunLevel::STARTING) ? "Initial State" : "Updated")
                                << std::endl;

                            for (const auto &entry : m_current_adapters)
                            {
                                const net::AdapterInfo &adapter = entry.second;

                                std::cout << "   Adapter " << adapter.system_name << std::endl;
                                std::cout << "      User Name    : " << adapter.user_name << std::endl;
                                std::cout << "      Description  : " << adapter.description << std::endl;
                                std::cout << "      MAC Address  : " << adapter.mac_address << std::endl;

                                for (const std::string &address: adapter.ip_addresses)
                                    std::cout << "      IP Address   : " << address << std::endl;
                            }
                        }

                        if (GetLocalRunLevel() == RunLevel::STARTING)
                        {
                            NotifyStarted();
                        }
                        else
                        {
                            // TODO - change notification
                        }
                    }
                }
            }

            void NetworkChangeService::RequestAdapters()
            {
                // This function is run in a thread-pool thread
                // and is where we actually query the
                // new adapter status

                IpHelperAccess::QueryAdapters(m_detecting_adapters);
            }

        } // namespace bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox
