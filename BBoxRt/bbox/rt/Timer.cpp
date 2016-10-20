/**
* @file
*
* Implementation for the bbox::rt::Timer class.
*/

#include <bbox/rt/Timer.h>
#include <bbox/Assert.h>

#include <boost/bind.hpp>

namespace bbox {
    namespace rt {

        Timer::Timer(const std::string &name, Service &parent)
            : Resource(name, parent)
            , m_timer(GetProactor().GetIoService())
            , m_pending_timeouts(0)
            , m_cur_seq(1)
            , m_wait_seq(0)
			, m_times_expired(0)
        {
        }

        Timer::Timer(const std::string &name, Resource &parent)
            : Resource(name, parent)
            , m_timer(GetProactor().GetIoService())
            , m_pending_timeouts(0)
            , m_cur_seq(1)
            , m_wait_seq(0)
			, m_times_expired(0)
        {
        }
        
        Timer::~Timer()
        {
        }

        void Timer::SetHandler(boost::function<void()> callback)
        {
            BBOX_ASSERT(m_cur_seq > m_wait_seq);

            m_callback = std::move(callback);
        }

        void Timer::StartSingleShot(const TimeSpan &delay)
        {
            m_pending_timeouts++;
            ++m_cur_seq;
            m_wait_seq = m_cur_seq;
            m_is_periodic = false;
			m_delay = delay;

            m_timer.expires_from_now(delay.m_duration);

            m_timer.async_wait(boost::bind(
                &Timer::HandleTimeout,
                this,
                m_wait_seq,
                _1));
        }

        void Timer::StartPeriodic(const TimeSpan &first, const TimeSpan &period, uint64_t num_times)
        {
            if (num_times == 0)
            {
                Cancel();
            }
            else
            {
                StartSingleShot(first);
                m_is_periodic = true;
                m_period = period;
                m_periodic_num_times_remaining = num_times;
                if (m_periodic_num_times_remaining < RUN_FOR_EVER)
                    --m_periodic_num_times_remaining;
            }
        }

        void Timer::Cancel()
        {
            if (m_wait_seq == m_cur_seq)
                m_timer.cancel();

            m_wait_seq = 0;
        }

        bool Timer::IsRunning() const
        {
            return (m_wait_seq == m_cur_seq);
        }

        void Timer::HandleTimeout(uint64_t seq, const boost::system::error_code &ec)
        {
            BBOX_ASSERT(m_pending_timeouts > 0);
            --m_pending_timeouts;

            if (!ec
                && (m_wait_seq == m_cur_seq)
                && (m_wait_seq == seq))
            {
                // Mark as not running

                m_wait_seq = 0;

                // Restart if periodic, not shutting down
                // and there are still times remaining

                if (m_is_periodic
                    && (GetLocalRunLevel() != RunLevel::STOPPING)
                    && (m_periodic_num_times_remaining > 0))
                {
                    m_pending_timeouts++;
                    ++m_cur_seq;
                    m_wait_seq = m_cur_seq;

                    if (m_periodic_num_times_remaining < RUN_FOR_EVER)
                        --m_periodic_num_times_remaining;

                    m_timer.expires_from_now(m_period.m_duration);

                    m_timer.async_wait(boost::bind(
                        &Timer::HandleTimeout,
                        this,
                        m_wait_seq,
                        _1));
                }

                // Notify the caller of the expiry

				m_times_expired += 1;

                if (m_callback)
                    m_callback();
            }

            CheckShutdown();
        }

        void Timer::HandleStopping()
        {
            // All timers must have been stopped by
            // their owner before being stopped

            BBOX_ASSERT(!IsRunning());

            CheckShutdown();
        }

        void Timer::CheckShutdown()
        {
            if ((GetLocalRunLevel() == RunLevel::STOPPING)
                && (m_pending_timeouts == 0))
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

		void Timer::PrintState(bbox::DebugOutput &out) const
		{
			out.Format("Times Expired: %d\n", m_times_expired);
			out.Format("Pending timeouts: %s\n", m_pending_timeouts);
			out.Format("Cur Seq: %d\n", m_cur_seq);
			out.Format("Wait Seq: %d\n", m_wait_seq);
			out.Format("Is Running: %s\n", IsRunning());

			if (IsRunning())
			{
				out.Format("Delay: %s\n", m_delay.ToString());
				out.Format("Is Periodic: %s\n", m_is_periodic);
				
				if (m_is_periodic)
				{
					out.Format("Period: %s\n", m_period.ToString());
					out.Format("Num Times Remaining: %d\n", m_periodic_num_times_remaining);
				}
			}
		}

    } // namespace bbox::rt
} // namespace bbox
