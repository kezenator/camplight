/**
 * @file
 *
 * Header for the bbox::rt::TimeSpan class.
 */

#ifndef __BBOX__RT__TIME_SPAN_H__
#define __BBOX__RT__TIME_SPAN_H__

#pragma once

#include <string>
#include <chrono>

namespace bbox {
    namespace rt {

        // Forward declaration
        class Timer;

        /**
         * A time span for a setting timers.
         */
        class TimeSpan
        {
            friend class Timer;

        public:
            TimeSpan()
                : m_duration()
            {
            }

            TimeSpan(const TimeSpan &other)
                : m_duration(other.m_duration)
            {
            }

            template <typename Rep, typename Ratio>
            explicit TimeSpan(const std::chrono::duration<Rep, Ratio> &duration)
                : m_duration(duration)
            {
            }

            ~TimeSpan()
            {
            }

            TimeSpan &operator =(const TimeSpan &other)
            {
                m_duration = other.m_duration;
                return *this;
            }

            static TimeSpan Hours(std::chrono::hours::rep hours)
            {
                return TimeSpan(std::chrono::hours(hours));
            }

            static TimeSpan Minutes(std::chrono::minutes::rep minutes)
            {
                return TimeSpan(std::chrono::minutes(minutes));
            }

            static TimeSpan Seconds(std::chrono::seconds::rep seconds)
            {
                return TimeSpan(std::chrono::seconds(seconds));
            }

            static TimeSpan Milliseconds(std::chrono::milliseconds::rep milliseconds)
            {
                return TimeSpan(std::chrono::milliseconds(milliseconds));
            }

            static TimeSpan Microseconds(std::chrono::microseconds::rep microseconds)
            {
                return TimeSpan(std::chrono::microseconds(microseconds));
            }

            std::string ToString() const;

            auto ToMilliseconds() const
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count();
            }
            
            bool operator ==(const TimeSpan &other) const { return m_duration == other.m_duration; }
            bool operator !=(const TimeSpan &other) const { return m_duration != other.m_duration; }

        private:

            std::chrono::steady_clock::duration m_duration;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__TIMER_H__