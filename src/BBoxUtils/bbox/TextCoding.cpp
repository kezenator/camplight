/**
 * @file
 *
 * Implementation file for the bbox::TextCoding class.
 */

#include <vector>

#include <bbox/TextCoding.h>
#include <bbox/Exception.h>
#include <bbox/Assert.h>

#include <cstring>
#include <sstream>

#include <boost/algorithm/string.hpp>

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

namespace bbox {


    Error TextCoding::ExternalBytes_to_UTF8(const std::vector<std::uint8_t> &from, std::string &to)
    {
        // TODO - only supporting UTF-8 for now - and without even
        // error checking

        if (from.size() == 0)
        {
            to.resize(0);
            return Error::Success;
        }

        to.resize(from.size());
        memcpy(&to[0], &from[0], from.size());

        return Error::Success;
    }

    Error TextCoding::UTF8_to_ExternalBytes(const std::string &from, std::vector<std::uint8_t> &to)
    {
        // For now - we just support the UTF-8 output format
        
        if (from.size() == 0)
        {
            to.resize(0);
            return Error::Success;
        }

        to.resize(from.size());
        memcpy(&to[0], &from[0], from.size());

        return Error::Success;
    }

    std::string TextCoding::Newlines_DOS_to_UNIX(const std::string &from)
    {
        return boost::algorithm::replace_all_copy(from, "\r\n", "\n");
    }

    std::string TextCoding::Newlines_UNIX_to_DOS(const std::string &from)
    {
		std::stringstream stream;

		auto size = from.size();
		const char *bytes = from.c_str();
		for (size_t i = 0; i < size; ++i)
		{
			char ch = bytes[i];

			if (ch == '\n')
			{
				stream << "\r\n";
			}
			else if (ch == '\r')
			{
				if (((i + 1) < size)
					&& (bytes[i + 1] == '\n'))
				{
					// It's already a "\r\n" - insert
					// and skip and extra char
					stream << "\r\n";
					i += 1;
				}
				else
				{
					stream << ch;
				}
			}
			else
			{
				stream << ch;
			}
		}

		return stream.str();

        return boost::algorithm::replace_all_copy(
            boost::algorithm::replace_all_copy(from, "\r\n", "\n"),
            "\n",
            "\r\n");
    }

#ifdef WIN32

    Error TextCoding::Win32_UTF8_to_UTF16(const std::string &from, std::wstring &to)
    {
        // We're receiving a "multi-byte" (i.e. UTF-8) string, and need to
        // convert it to a "wide-string" (i.e. UTF-16) format.

        // First, call MultiByteToWideChar to get the number of output
        // wide chars required

        int int_from_size_plus_one = int(from.size() + 1);
        BBOX_ASSERT(int_from_size_plus_one == (from.size() + 1));

        int result1 = ::MultiByteToWideChar(
                            CP_UTF8,
                            MB_ERR_INVALID_CHARS,
                            from.c_str(),
                            int_from_size_plus_one,
                            NULL,
                            0);

        // Docs says that U+FFFD is returned if the
        // encoding of the from string if invalid

        if (result1 == 0xFFFD)
        {
            // TODO - maybe handle this error - perhaps testing required
            return std::errc::argument_out_of_domain;
        }

        // Result returns zero if not successful,
        // with GetLastError() returning more details

        if (result1 == 0)
            return Error::Win32_GetLastError();

        // OK - we're got the correct length - now
        // perform the real conversion

        to.resize(result1);

        int result2 = ::MultiByteToWideChar(
                            CP_UTF8,
                            MB_ERR_INVALID_CHARS,
                            from.c_str(),
                            int_from_size_plus_one,
                            &to[0],
                            result1);

        // Must check same error conditions

        if (result2 == 0xFFFD)
        {
            return std::errc::argument_out_of_domain;
        }

        if (result2 == 0)
        {
            return Error::Win32_GetLastError();
        }

        BBOX_ASSERT(result1 == result2);
        BBOX_ASSERT(to.size() == result2);
        BBOX_ASSERT(to.size() > 0);
        BBOX_ASSERT(to[to.size() - 1] == 0);

        to.resize(to.size() - 1);

        return Error::Success;
    }

    std::wstring TextCoding::Win32_UTF8_to_UTF16(const std::string &from)
    {
        std::wstring to;

        Error err = Win32_UTF8_to_UTF16(from, to);

        if (err)
        {
            throw Exception(err);
        }

        return to;
    }

    Error TextCoding::Win32_UTF16_to_UTF8(const std::wstring &from, std::string &to)
    {
        // We're reciving a "wide-char" (i.e. UTF-16) string, and need to
        // convert it to a "multi-byte" (i.e. UTF-8) format

        // First, call WideCharToMultiByte to get the number of output
        // bytes required

        int int_from_size_plus_one = int(from.size() + 1);
        BBOX_ASSERT(int_from_size_plus_one == (from.size() + 1));

        int result1 = ::WideCharToMultiByte(
                            CP_UTF8,
                            WC_ERR_INVALID_CHARS,
                            from.c_str(),
                            int_from_size_plus_one,
                            NULL,
                            0,
                            NULL,
                            NULL);

        // Handle the two error cases -
        // docs say 0xFFFD for invalid UTF encodings,
        // plus zero for other errors

        if (result1 == 0xFFFD)
        {
            return std::errc::argument_out_of_domain;
        }

        if (result1 == 0)
        {
            return Error::Win32_GetLastError();
        }

        // OK - we're got the correct length - now
        // perform the real conversion

        to.resize(result1);

        int result2 = ::WideCharToMultiByte(
                            CP_UTF8,
                            WC_ERR_INVALID_CHARS,
                            from.c_str(),
                            int_from_size_plus_one,
                            &to[0],
                            result1,
                            NULL,
                            NULL);

        // Must check same error conditions

        if (result2 == 0xFFFD)
        {
            return std::errc::argument_out_of_domain;
        }

        if (result2 == 0)
        {
            return Error::Win32_GetLastError();
        }

        BBOX_ASSERT(result1 == result2);
        BBOX_ASSERT(to.size() == result2);
        BBOX_ASSERT(to.size() > 0);
        BBOX_ASSERT(to[to.size() - 1] == 0);

        to.resize(to.size() - 1);

        return Error::Success;
    }

    std::string TextCoding::Win32_UTF16_to_UTF8(const std::wstring &from)
    {
        std::string to;

        Error err = Win32_UTF16_to_UTF8(from, to);

        if (err)
        {
            throw Exception(err);
        }

        return to;
    }

#endif // WIN32

} // namespace bbox
