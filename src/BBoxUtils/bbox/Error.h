/**
 * @file
 *
 * Header file for the bbox::Error class.
 */

#ifndef __BBOX__ERROR_H__
#define __BBOX__ERROR_H__

#pragma once

#include <boost/system/system_error.hpp>
#include <system_error>
#include <string>
#include <iostream>

namespace bbox {

    class Error
    {
    public:

        enum success_t
        {
            Success
        };

        Error()
            : m_err_code()
        {
        }

        Error(const Error &other)
            : m_err_code(other.m_err_code)
        {
        }

        Error(success_t)
            : m_err_code(0, boost::system::generic_category())
        {
        }

        Error(boost::system::errc::errc_t errc)
            : m_err_code(static_cast<int>(errc), boost::system::generic_category())
        {
        }

        Error(std::errc errc)
            : m_err_code(static_cast<int>(errc), boost::system::generic_category())
        {
            // Boost docs say that for the generic category for C++ 11,
            // the boost::system::errc::errc_t enum lines up
            // with the std::errc_t enum values.
        }

        Error(boost::system::error_code errc)
            : m_err_code(errc)
        {
        }

        Error(boost::system::error_condition errc)
            : m_err_code(errc.value(), errc.category())
        {
        }

        template <typename ErrorCodeEnum>
        Error(ErrorCodeEnum enum_val)
            : m_err_code(enum_val)
        {
        }

        ~Error()
        {
        }

        Error &operator =(const Error &other)
        {
            m_err_code = other.m_err_code;
            return *this;
        }

        explicit operator bool() const
        {
            return bool(m_err_code);
        }

        std::string ToString() const;

        boost::system::error_code ToErrorCode() const
        {
            return m_err_code;
        }

        bool operator ==(success_t val) const { return !operator bool(); }
        bool operator !=(success_t val) const { return operator bool(); }

        bool operator ==(const Error &other) const { return m_err_code == other.m_err_code; }
        bool operator !=(const Error &other) const { return m_err_code != other.m_err_code; }

#ifdef WIN32

        static Error Win32_GetLastError();
		static Error Win32_FromHRESULT(uint32_t hresult);

#else // !WIN32

        static Error posix_errno();

#endif // !WIN32

    private:
        boost::system::error_code m_err_code;
    };

    inline std::ostream &operator <<(std::ostream &stream, const Error &err)
    {
        return stream << err.ToString();
    }

} // namespace bbox

#endif // __BBOX__ERROR_H__
