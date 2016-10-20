/**
 * @file
 *
 * Header file for the bbox::TextCoding class.
 */

#ifndef __BBOX__TEXT_CODING_H__
#define __BBOX__TEXT_CODING_H__

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <bbox/Error.h>

namespace bbox {

    /**
     * Provides access to all text encoding functions
     * via a number of static access methods.
     */
    class TextCoding
    {
        // Instances are not allowed
        TextCoding() = delete;
        TextCoding(const TextCoding &) = delete;
        void operator =(const TextCoding &) = delete;

    public:

        /**
         * Converts an external byte-sequence into a UTF-8 internal string.
         * The external byte sequence could be a UTF-8 string, or a UTF-16 or
         * UTF-32 string with correct byte ordering marks, or a local
         * system code page string. The algorithm used to determin this
         * is indeterminate - a best effort attempt is made.
         *
         * @param from the external byte-sequence.
         * @param to the result.
         * @return an error, or success.
         */
        static Error ExternalBytes_to_UTF8(const std::vector<std::uint8_t> &from, std::string &to);

        /**
        * Converts an internal UTF-8 string into a byte-sequence
        * that can be saved to disk on the local system. This should be
        * a Unicode format - indicated either by a UTF-16 BE or LE mark,
        * or is encoded in some system-specific form. The algorithm
        * used to determin this is indeterminaite - a best effort
        * attempt is made.
        *
        * @param from the external byte-sequence.
        * @param to the result.
        * @return an error, or success.
        */
        static Error UTF8_to_ExternalBytes(const std::string &from, std::vector<std::uint8_t> &to);

#ifdef WIN32
        /**
         * Converts a UTF-8 internal string representation
         * into a Win32 UTF-16 string.
         *
         * Only available on Windows.
         *
         * @param from the UTF-8 string to convert.
         * @param to the UTf-16 result
         * @return the error, or success.
         */
        static Error Win32_UTF8_to_UTF16(const std::string &from, std::wstring &to);

        /**
         * Converts a UTF-8 internal string representation
         * into a Win32 UTF-16 string, or throws an exception
         * on error.
         *
         * Only available on Windows.
         *
         * @param from the UTF-8 string to convert.
         * @return a UTF-16 equivelent.
         */
        static std::wstring Win32_UTF8_to_UTF16(const std::string &from);

        /**
         * Converts a UTF-16 string representation
         * into a Win32 UTF-8 internal string.
         *
         * Only available on Windows.
         *
         * @param from the UTF-16 string to convert.
         * @param to the UTf-8 result
         * @return the error, or success.
         */
        static Error Win32_UTF16_to_UTF8(const std::wstring &from, std::string &to);

        /**
         * Converts a UTF-16 string representation
         * into a Win32 UTF-8 internal string, or throws an exception
         * on error.
         *
         * Only available on Windows.
         *
         * @param from the UTF-16 string to convert.
         * @return a UTF-8 equivelent.
         */
        static std::string Win32_UTF16_to_UTF8(const std::wstring &from);
#endif // WIN32
    };

}

#endif // __BBOX__TEXT_CODING_H__
