/**
* @file
*
* Implementation for the bbox::rt::ThreadPoolRef class.
*/

#include <bbox/rt/ThreadPoolRef.h>
#include <bbox/Assert.h>
#include <bbox/Exception.h>

#include <boost/thread.hpp>

namespace bbox {
    namespace rt {

        ThreadPoolRef::ThreadPoolRef(const std::string &name, Service &parent)
            : ServiceReference<ThreadPool>(name, parent, ThreadPool::SERVICE_NAME)
            , m_items_in_progress(0)
            , m_items_completed(0)
        {
        }

        ThreadPoolRef::~ThreadPoolRef()
        {
            BBOX_ASSERT(m_items_in_progress == 0);
        }

        void ThreadPoolRef::HandleStarting()
        {
            BBOX_ASSERT(m_items_in_progress == 0);

            NotifyStarted();
        }

        void ThreadPoolRef::HandleStopping()
        {
            CheckStopped();
        }

        void ThreadPoolRef::PrintState(bbox::DebugOutput &out) const
        {
            out.Format("Items in progress: %d\n", m_items_in_progress);
            out.Format("Items completed:   %d\n", m_items_completed);
        }

        void ThreadPoolRef::CheckStopped()
        {
            if ((GetLocalRunLevel() == RunLevel::STOPPING)
                && (m_items_in_progress == 0))
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

        void ThreadPoolRef::RunOnThread(boost::function<void()> &&function,
                                        boost::function<void(const Error &err, const std::string &what)> &&completion_handler)
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);

            m_items_in_progress += 1;

            (*this)->Post(
                std::move(function),
                [this, completion_handler](const Error &err, const std::string &what)
                {
                    BBOX_ASSERT(m_items_in_progress > 0);

                    m_items_in_progress -= 1;
                    m_items_completed += 1;

                    completion_handler(err, what);

                    CheckStopped();
                });
        }

    } // namespace bbox::rt
} // namespace bbox
