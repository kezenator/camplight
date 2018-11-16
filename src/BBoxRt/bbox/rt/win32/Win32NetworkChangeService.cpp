/**
* @file
*
* Implementation for the bbox::rt::win32::Win32NetworkChangeService class.
*/

#ifdef WIN32

#include <bbox/rt/win32/Win32NetworkChangeService.h>
#include <bbox/rt/win32/IpHelperAccess.h>
#include <bbox/Assert.h>
#include <bbox/ScopedDebugIndent.h>
#include <bbox/enc/ToDebugOutput.h>

#include <boost/bind.hpp>

namespace {

    void Callback(void *context)
    {
        bbox::rt::win32::Win32NetworkChangeService *service = static_cast<bbox::rt::win32::Win32NetworkChangeService *>(context);

        service->TriggerUpdate();
    }

} // annonymous namespace

namespace bbox {
    namespace rt {
        namespace win32 {

			Win32NetworkChangeService::Win32NetworkChangeService(const std::string &name, Proactor &parent)
                : net::NetworkChangeService(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_change_handle(nullptr)
                , m_pending_counter(0)
                , m_another_update_required(false)
            {
                SetThisDependantOn(m_thread_pool_ref);
            }

			Win32NetworkChangeService::Win32NetworkChangeService(const std::string &name, Service &parent)
                : net::NetworkChangeService(name, parent)
                , m_thread_pool_ref("thread-pool-ref", *this)
                , m_change_handle(nullptr)
                , m_pending_counter(0)
                , m_another_update_required(false)
            {
                SetThisDependantOn(m_thread_pool_ref);
            }

			Win32NetworkChangeService::~Win32NetworkChangeService()
            {
            }

            void Win32NetworkChangeService::HandleStarting()
            {
                // Register for change notifications

                IpHelperAccess::RegisterChangeNotifier(
                    m_change_handle,
                    &Callback,
                    this);
            }

            void Win32NetworkChangeService::HandleStopping()
            {
                // To start the shutdown process, we want to
                // destroy the change notifier
                // and start shutting down the thread pool access

                IpHelperAccess::DestroyChangeNotifier(m_change_handle);
                m_thread_pool_ref.RequestStop(boost::bind(&Win32NetworkChangeService::CheckShutdown, this));

                CheckShutdown();
            }

            void Win32NetworkChangeService::CheckShutdown()
            {
                if ((GetLocalRunLevel() == RunLevel::STOPPING)
                    && (m_thread_pool_ref.GetOverallRunLevel() == RunLevel::STOPPED)
                    && (0 == m_pending_counter))
                {
                    NotifyStopped();
                    RequestStopAllChildren();
                }
            }

			void Win32NetworkChangeService::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Current adapters: %d\n", GetCurrentAdapterInfo().size());

				ScopedDebugIndent indent(out, 4);
                bbox::enc::ToDebugOutput(out, GetCurrentAdapterInfo());
			}

            void Win32NetworkChangeService::TriggerUpdate()
            {
                // This function is called from a random
                // thread created by the IpHelper API.

                // Increment the pending count
                // and thread-safe post a function to be run
                // back in the main thread.

                m_pending_counter.fetch_add(1, boost::memory_order_relaxed);

                GetProactor().ThreadSafePost(boost::bind(
                    &Win32NetworkChangeService::HandleUpdateRequired,
                    this));
            }

            void Win32NetworkChangeService::HandleUpdateRequired()
            {
                // Decrement the pending operation count

                m_pending_counter.fetch_sub(1, boost::memory_order_release);

                // There there's no thread pool operation running,
                // then start one - otherwise mark another update
                // as required

                if (!m_thread_pool_ref.AnyOperationsRunning())
                {
                    m_thread_pool_ref.RunOnThread(
                        boost::bind(&Win32NetworkChangeService::RequestAdapters, this),
                        boost::bind(&Win32NetworkChangeService::HandlUpdateComplete, this));
                }
                else
                {
                    m_another_update_required = true;
                }
            }

            void Win32NetworkChangeService::HandlUpdateComplete()
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
                        boost::bind(&Win32NetworkChangeService::RequestAdapters, this),
                        boost::bind(&Win32NetworkChangeService::HandlUpdateComplete, this));
                }
                else
                {
                    if ((m_detecting_adapters != GetCurrentAdapterInfo())
                        || (GetLocalRunLevel() == RunLevel::STARTING))
                    {
                        // We've got new settings - save them, and either
                        // mark us as started or post a change notification

                        bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Activity);
                        if (out)
                        {
                            out << "Network Adapters - "
                                << ((GetLocalRunLevel() == RunLevel::STARTING) ? "Initial State" : "Updated")
                                << std::endl;

                            ScopedDebugIndent indent(out, 4);
                            bbox::enc::ToDebugOutput(out, m_detecting_adapters);
                        }

						ReportChange(std::move(m_detecting_adapters));

                        if (GetLocalRunLevel() == RunLevel::STARTING)
                        {
                            NotifyStarted();
                        }
                    }
                }
            }

            void Win32NetworkChangeService::RequestAdapters()
            {
                // This function is run in a thread-pool thread
                // and is where we actually query the
                // new adapter status

                IpHelperAccess::QueryAdapters(m_detecting_adapters);
            }

        } // namespace bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#endif // WIN32