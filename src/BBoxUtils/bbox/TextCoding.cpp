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
        return Newlines_Implementation(from, "\n", 1);
    }

    std::string TextCoding::Newlines_UNIX_to_DOS(const std::string &from)
    {
        return Newlines_Implementation(from, "\r\n", 2);
    }

	std::string TextCoding::Newlines_Implementation(const std::string &from, const char *replace, size_t replace_length)
	{
        std::string result;

        // Reserve space for half the characters to be replaced with a longer
        // equivilent (if replace_length is greater than 1)

        result.reserve(from.size() + (from.size() / 2 * (replace_length - 1)));

        // Keep appending non-newline sections
        // and then perform the appropriate replacement
        // for new lines found.

        size_t start_pos = 0;
        while (start_pos < from.size())
        {
	        size_t next_pos = from.find_first_of("\r\n", start_pos);

	        if (next_pos == std::string::npos)
	        {
		        // The range from start_pos to the end of the string
		        // contains no newlines - it can be appended

		        result.append(from, start_pos, from.size() - start_pos);

		        start_pos = from.size();
	        }
	        else
	        {
		        // We've found a new-line further into the string.
		        // We need to:
		        // 1) Append any non-new-line characters between the start
		        //    pos and the found pos
		        // 2) Append the correct replacement newline characters
		        // 3) Move past the new line character sequence - \r, \n, or \r\n.

		        result.append(from, start_pos, next_pos - start_pos);
		        result.append(replace, replace_length);

		        start_pos = next_pos + 1;

		        if ((from[next_pos] == '\r')
			        && (start_pos < from.size())
			        && (from[start_pos] == '\n'))
		        {
			        // The from string contains a \r\n sequence -
			        // skip both

			        start_pos += 1;
		        }
	        }
        }

        return result;
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
