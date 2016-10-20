/**
* @file
*
* Header for the bbox::xml::ErrorCategory class.
*/

#ifndef __BBOX__XML__ERROR_H__
#define __BBOX__XML__ERROR_H__

#pragma once

#include <bbox/Error.h>
#include <tinyxml2/tinyxml2.h>

namespace bbox {
    namespace xml {

        boost::system::error_category &ErrorCategory();

    } // namespace bbox::xml
} // namespace bbox

namespace boost {
    namespace system {

        template <>
        struct is_error_code_enum<tinyxml2::XMLError>
        {
            static const bool value = true;
        };

    } // namespace boost::system
} // namespace boost

namespace tinyxml2 {

    inline boost::system::error_code make_error_code(tinyxml2::XMLError e)
    {
        return boost::system::error_code(static_cast<int>(e), bbox::xml::ErrorCategory());
    }

} // namespace tinyxml2

#endif // __BBOX__XML__ERROR_H__
