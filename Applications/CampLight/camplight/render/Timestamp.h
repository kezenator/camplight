/**
 * @file
 *
 * Header file for the camplight::render::Timestamp class.
 */

#ifndef __CAMPLIGHT__RENDER__TIMESTAMP_H__
#define __CAMPLIGHT__RENDER__TIMESTAMP_H__

#include <string.h>
#include <bbox/rt/TimeSpan.h>

namespace camplight {
    namespace render {

        class Timestamp
        {
        public:
            Timestamp() : m_value(0) {}
            Timestamp(const Timestamp &other) = default;
            Timestamp &operator =(const Timestamp &other) = default;

            explicit Timestamp(size_t value) : m_value(value) {}

            size_t ToMilliseconds() const
            {
                return m_value;
            }

            Timestamp RelativeToStart(const Timestamp &start) const
            {
                return Timestamp(m_value - start.m_value);
            }

        private:
            size_t m_value;
        };
        
    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__TIMESTAMP_H__
