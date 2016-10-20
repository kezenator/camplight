/**
 * @file
 *
 * Header file for the bbox::Format function.
 */

#ifndef __BBOX_FORMAT_H__
#define __BBOX_FORMAT_H__

#pragma once

#include <string>
#include <sstream>

namespace bbox {

    namespace details {

        struct FormatFlags
        {
            FormatFlags(std::ostream &_stream)
                : stream(_stream)
                , is_char(false)
            {
            }

            std::ostream &stream;
            bool is_char;
        };

        bool FormatFindNext(const char *&format, FormatFlags &flags);
        void FormatDummyRest(const char *format, FormatFlags &flags);

    }

    void Format(std::ostream &stream, const char *format);

    inline std::string Format(const char *format)
    {
        std::stringstream stream;

        Format(stream, format);

        return stream.str();
    }

    template <typename Type, typename... Rest>
    inline void Format(std::ostream &stream, const char *format, const Type &val, Rest... rest)
    {
        details::FormatFlags flags(stream);

        if (details::FormatFindNext(format, flags))
        {
            stream << val;
            Format(stream, format, rest...);
        }
    }

    // char specialization
    template <typename... Rest>
    inline void Format(std::ostream &stream, const char *format, const char &val, Rest... rest)
    {
        details::FormatFlags flags(stream);

        if (details::FormatFindNext(format, flags))
        {
            if (flags.is_char)
                stream << val;
            else
                stream << (int)val;

            Format(stream, format, rest...);
        }
    }

    // unsigned char specialization
    template <typename... Rest>
    inline void Format(std::ostream &stream, const char *format, const unsigned char &val, Rest... rest)
    {
        details::FormatFlags flags(stream);

        if (details::FormatFindNext(format, flags))
        {
            if (flags.is_char)
                stream << val;
            else
                stream << (unsigned int)val;

            Format(stream, format, rest...);
        }
    }

    // bbox::Error specialization
    // TODO : anything with a ToString() or str() or c_str() method

    template <typename... Rest>
    inline std::string Format(const char *format, Rest... rest)
    {
        std::stringstream stream;

        Format(stream, format, rest...);

        return stream.str();
    }

}

#endif // __BBOX_FORMAT_H__
