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
            , m_pending(false)
            , m_callback(std::move(callback))
        {
        }

        OneShotWork::OneShotWork(const std::string &name, Service &parent, boost::function<void()> &&callback)
            : Resource(name, parent)
            , m_pending(false)
            , m_callback(std::move(callback))
        {
        }

        OneShotWork::~OneShotWork()
        {
        }

        void OneShotWork::Schedule()
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);

            if (!m_pending)
            {
                m_pending = true;
                GetProactor().Post(boost::bind(&OneShotWork::DoCallback, this));
            }
        }

        void OneShotWork::HandleStopping()
        {
            if (!m_pending)
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

		void OneShotWork::PrintState(bbox::DebugOutput &out) const
		{
			out.Format("Pending: %s\n", m_pending);
		}

        void OneShotWork::DoCallback()
        {
            BBOX_ASSERT(m_pending);

            m_pending = false;

            if (m_callback)
                m_callback();

            if (!m_pending
                && (GetLocalRunLevel() == RunLevel::STOPPING))
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

    } // namespace bbox::rt
} // namespace bbox
