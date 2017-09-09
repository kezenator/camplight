/**
 * @file
 *
 * Implementation file for the bbox::Error class.
 */

#include <bbox/Error.h>
#include <bbox/Format.h>

#ifdef WIN32
#include <Windows.h>
#include <bbox/TextCoding.h>
#else // not WIN32
#include <vector>
#include <string.h>
#endif // not WIN32

namespace bbox {

    std::string Error::ToString() const
    {
        std::string result;

        if (*this)
        {
            result = Format("[\"%s\"/%s/%d(0x%08X)]",
                m_err_code.message(),
                m_err_code.category().name(),
                m_err_code.value(),
                (unsigned int)m_err_code.value());
        }
        else
        {
            result = "[Success]";
        }

        return result;
    }

#ifdef WIN32

    namespace {

        class win32_category_t :
            public boost::system::error_category
        {
            const char* name() const noexcept override
            {
                return "Win32";
            }

            boost::system::error_condition default_error_condition(int val) const noexcept override
            {
                // TODO
                if (val == 0)
                    return boost::system::error_condition(0, boost::system::generic_category());
                else
                    return boost::system::error_condition(boost::system::errc::not_supported);
            }

            std::string message(int val) const noexcept override
            {
                // First, convert the error code into a wide-char string
                // using FormatMessageW

                // TODO - wrap allocated memory in RAII
                // to get rid of lost memory in case of exceptions
                // in std::wstring etc.

                LPVOID msg_buf;

                DWORD length = ::FormatMessageW(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    0,
                    DWORD(val),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    reinterpret_cast<LPWSTR>(&msg_buf),
                    0,
                    NULL);

                if (length == 0)
                {
                    return "Unknown Win32 Error";
                }

                std::wstring wstring_buf;
                wstring_buf.resize(length);
                memcpy(&wstring_buf[0], msg_buf, length * sizeof(wchar_t));
                ::LocalFree(msg_buf);

                // Now, convert the wstring back into a UTF-8 string

                std::string result;
                Error err = TextCoding::Win32_UTF16_to_UTF8(wstring_buf, result);

                if (err)
                {
                    return "Unknown Win32 Error (error converting UTF16 -> UTF8)";
                }

                // Finally, remove any trailing new line characters
                // as these seem to be returned by the Win32 functions

                if ((result.size() > 0)
                    && (result[result.size() - 1] == '\n'))
                {
                    result.resize(result.size() - 1);
                }

                if ((result.size() > 0)
                    && (result[result.size() - 1] == '\r'))
                {
                    result.resize(result.size() - 1);
                }

                return result;
            }
        };

        win32_category_t win32_category;

    } // annonymous namespace

    Error Error::Win32_GetLastError()
    {
        static_assert(sizeof(DWORD) == sizeof(int), "Win32 DWORD is not the same size as int");

        // Get the last error code from the windows
        // call

        DWORD win32_code = ::GetLastError();

        return boost::system::error_code(win32_code, win32_category);
    }

#else // not WIN32

    namespace {

        class errno_category_t :
            public boost::system::error_category
        {
            const char* name() const noexcept
            {
                return "errno";
            }

            boost::system::error_condition default_error_condition(int val) const noexcept
            {
                // TODO
                if (val == 0)
                    return boost::system::error_condition(0, boost::system::generic_category());
                else
                    return boost::system::error_condition(boost::system::errc::not_supported);
            }

            std::string message(int val) const
            {
                std::vector<char> buffer;
                buffer.resize(1024);

                return std::string(strerror_r(val, &buffer[0], buffer.size()));
            }
        };

        errno_category_t errno_category;

    } // annonymous namespace

    Error Error::posix_errno()
    {
        return boost::system::error_code(errno, errno_category);
    }

#endif // WIN32


} // namespace bbox
