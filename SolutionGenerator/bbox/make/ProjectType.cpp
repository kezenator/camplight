/**
* @file
*
* Implementation file for the bbox::make::ProjectType class.
*/

#include <bbox/make/ProjectType.h>
#include <boost/algorithm/string/predicate.hpp>

namespace bbox {

    namespace make {

        std::string
        ProjectType::ToString() const
        {
            const char *str = "Unknown";

            switch (m_value)
            {
            case Application: str = "Application"; break;
            case StaticLibrary: str = "StaticLibrary"; break;
            case UnitTests: str = "UnitTests"; break;
            case TypeScriptApplication: str = "TypeScriptApplication"; break;
            case TypeScriptLibrary: str = "TypeScriptLibrary"; break;
            }

            return str;
        }

        bool
        ProjectType::FromString(const std::string &str, ProjectType &val)
        {
            val = ProjectType();

            if (boost::iequals(str, "Application"))
            {
                val = Application;
                return true;
            }
            else if (boost::iequals(str, "StaticLibrary"))
            {
                val = StaticLibrary;
                return true;
            }
            else if (boost::iequals(str, "UnitTests"))
            {
                val = UnitTests;
                return true;
            }
            else if (boost::iequals(str, "TypeScriptApplication"))
            {
                val = TypeScriptApplication;
                return true;
            }
            else if (boost::iequals(str, "TypeScriptLibrary"))
            {
                val = TypeScriptLibrary;
                return true;
            }

            return false;
        }

    } // namespace bbox::make
} // namespace bbox
