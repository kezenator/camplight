/**
 * @file
 *
 * Header for the bbox::rt::Timer class.
 */

#ifndef __BBOX__RT__TIMER_H__
#define __BBOX__RT__TIMER_H__

#pragma once

#include <bbox/rt/Resource.h>
#include <bbox/rt/TimeSpan.h>

#include <boost/asio/steady_timer.hpp>
#include <functional>

namespace bbox {
    namespace rt { 

        /**
         * Resource that provides timer services.
         */
        class Timer: public Resource
        {
        public:
            static const uint64_t RUN_FOR_EVER = UINT64_MAX;

            Timer(const std::string &name, Resource &parent, std::function<void()> &&handler = std::function<void()>());
            Timer(const std::string &name, Service &parent, std::function<void()> &&handler = std::function<void()>());
            ~Timer();

            void SetHandler(std::function<void()> &&callback);
            
            void StartSingleShot(const TimeSpan &delay);
            void StartPeriodic(const TimeSpan &period, uint64_t num_times = RUN_FOR_EVER) { StartPeriodic(period, period, num_times); }
            void StartPeriodic(const TimeSpan &first, const TimeSpan &period, uint64_t num_times = RUN_FOR_EVER);
            void Cancel();

            bool IsRunning() const;

        private:

            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;
            void CheckShutdown();

            void HandleTimeout(boost::uint64_t seq, const boost::system::error_code &ec);

            boost::asio::steady_timer m_timer;
            size_t m_pending_timeouts;
            uint64_t m_cur_seq;
            uint64_t m_wait_seq;
            std::function<void()> m_callback;
			TimeSpan m_delay;
            bool m_is_periodic;
            TimeSpan m_period;
            uint64_t m_periodic_num_times_remaining;
			uint64_t m_times_expired;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__TIMER_H__