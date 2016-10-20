/**
 * @file
 *
 * Header for the bbox::rt::RunLevel class.
 */

#ifndef __BBOX__RT__RUN_LEVEL_H__
#define __BBOX__RT__RUN_LEVEL_H__

#pragma once

#include <iosfwd>
#include <string>

namespace bbox {
    namespace rt { 

        /**
         * The run level (state) of a resource, proactor or service.
         */
        class RunLevel
        {
        public:

            enum E_LEVEL
            {
                CONSTRUCTED,
                STARTING,
                RUNNING,
                STOPPING,
                STOPPED,
            };

            RunLevel()
                : m_level(STOPPED)
            {
            }

            RunLevel(E_LEVEL level)
                : m_level(level)
            {
            }

            RunLevel(const RunLevel &other)
                : m_level(other.m_level)
            {
            }

            ~RunLevel()
            {
            }

            RunLevel &operator =(const RunLevel &other)
            {
                m_level = other.m_level;
                return *this;
            }

            E_LEVEL GetLevel() const
            {
                return m_level;
            }

            std::string ToString() const;

            bool operator ==(const RunLevel &other) const { return m_level == other.m_level; }
            bool operator !=(const RunLevel &other) const { return m_level != other.m_level; }

        private:
            E_LEVEL m_level;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__RUN_LEVEL_H__