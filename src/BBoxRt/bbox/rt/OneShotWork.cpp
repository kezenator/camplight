/**
* @file
*
* Implementation for the bbox::rt::OneShotWork class.
*/

#include <bbox/rt/OneShotWork.h>
#include <bbox/Assert.h>
#include <boost/bind.hpp>

namespace bbox {
    namespace rt {

        OneShotWork::OneShotWork(const std::string &name, Resource &parent, boost::function<void()> &&callback)
            : Resource(name, parent)
            , m_next_token(1)
            , m_pending_token(0)
            , m_last_resolved_token(0)
            , m_callback(std::move(callback))
        {
        }

        OneShotWork::OneShotWork(const std::string &name, Service &parent, boost::function<void()> &&callback)
            : Resource(name, parent)
            , m_next_token(1)
            , m_pending_token(0)
            , m_last_resolved_token(0)
            , m_callback(std::move(callback))
        {
        }

        OneShotWork::~OneShotWork()
        {
        }

        void OneShotWork::Schedule()
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);

            if (m_pending_token == 0)
            {
                m_pending_token = m_next_token;
                m_next_token += 1;
                GetProactor().Post(boost::bind(&OneShotWork::DoCallback, this));
            }
        }

        void OneShotWork::Cancel()
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);

            m_pending_token = 0;
        }

        void OneShotWork::HandleStopping()
        {
            m_pending_token = 0;
            CheckShutdown();
        }

        void OneShotWork::CheckShutdown()
        {
            if ((GetLocalRunLevel() == RunLevel::STOPPING)
                && ((m_last_resolved_token + 1) == m_next_token))
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

		void OneShotWork::PrintState(bbox::DebugOutput &out) const
		{
			out.Format("Schedled:       %s\n", IScheduled());
            out.Format("Next Token:     %d\n", m_next_token);
            out.Format("Pending Token:  %d\n", m_pending_token);
            out.Format("Resolve Token:  %d\n", m_last_resolved_token);
        }

        void OneShotWork::DoCallback()
        {
            BBOX_ASSERT((m_last_resolved_token + 1) < m_next_token);

            m_last_resolved_token += 1;

            if (m_pending_token == m_last_resolved_token)
            {
                m_pending_token = 0;

                if (m_callback)
                    m_callback();
            }

            CheckShutdown();
        }

    } // namespace bbox::rt
} // namespace bbox
