/**
* @file
*
* Header file for the bbox::make::ProjectType class.
*/

#ifndef __BBOX__MAKE__PROJECT_TYPE_H__
#define __BBOX__MAKE__PROJECT_TYPE_H__

#pragma once

#include <string>

namespace bbox {

    namespace make {

        /**
         * Represents the type of a solution.
         */
        class ProjectType
        {
        public:

            enum E_TYPE
            {
                Application,
                StaticLibrary,
                UnitTests,
                TypeScriptApplication,
                TypeScriptLibrary,
            };

            static const int Invalid = UnitTests + 1;

            ProjectType()
                : m_value(E_TYPE(Invalid))
            {
            }

            ProjectType(const ProjectType &other)
                : m_value(other.m_value)
            {
            }

            ProjectType(E_TYPE type)
                : m_value(type)
            {
            }

            ProjectType &operator =(const ProjectType &other)
            {
                m_value = other.m_value;

                return *this;
            }

            ProjectType &operator =(E_TYPE type)
            {
                m_value = type;

                return *this;
            }

            E_TYPE GetValue() const
            {
                return m_value;
            }

            std::string ToString() const;
            static bool FromString(const std::string &str, ProjectType &val);

            inline bool operator ==(const ProjectType &other) const { return m_value == other.m_value; }
            inline bool operator !=(const ProjectType &other) const { return m_value != other.m_value; }

            inline bool operator ==(E_TYPE type) const { return m_value == type; }
            inline bool operator !=(E_TYPE type) const { return m_value != type; }

        private:
            E_TYPE m_value;
        };

    } // namespace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__PROJECT_TYPE_H__