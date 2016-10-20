/**
 * @file
 *
 * Implementation file for the bbox::Format function.
 */

#include <bbox/Format.h>
#include <bbox/Assert.h>

namespace bbox {

    namespace details {

        bool FormatFindNext(const char *&format, FormatFlags &flags)
        {
            // Print out all characters until we find
            // a percent sign, return false if end of string found

            {
                const char *orig_format = format;
                while (*format && (*format != '%'))
                    format++;

                flags.stream.write(orig_format, format - orig_format);

                if (*format == 0)
                    return false;
            }

            // OK - we've now printed all characters up to the next
            // percent sign - process the details until we find
            // a type specifier

            BBOX_ASSERT(*format == '%');
            format++;

            // Setup initial flags

            flags.is_char = false;

            std::ios_base::fmtflags stream_flags
                = std::ios_base::boolalpha
                | std::ios_base::dec
                | std::ios_base::fixed
                | std::ios_base::right;

            // First, accept the flags characters

            flags.stream.fill(' ');

            while (true)
            {
                bool exit_loop = false;

                switch (*format)
                {
                case '-':
                    stream_flags = (stream_flags & ~std::ios_base::adjustfield) | std::ios_base::left;
                    break;

                case '+':
                    stream_flags |= std::ios_base::showpos;
                    break;

                case ' ':
                    // Not supported in ios_base - ignore
                    break;

                case '#':
                    stream_flags |= std::ios_base::showpoint | std::ios_base::showbase;
                    break;

                case '0':
                    flags.stream.fill('0');
                    break;

                default:
                    // The next character is not a flag -
                    // break out of the loop
                    exit_loop = true;
                    break;
                }

                if (exit_loop)
                    break;
                format++;
            }

            // Now accept a width

            if ((*format >= '0')
                && (*format <= '9'))
            {
                std::streamsize width = 0;

                while ((*format >= '0')
                       && (*format <= '9'))
                {
                    width = (width * 10)
                        + (*format - '0');
                    format++;
                }

                flags.stream.width(width);
            }
            else
            {
                flags.stream.width(0);
            }

            // Now accept a dot and precision

            if (*format == '.')
            {
                format++;

                if ((*format >= '0')
                    && (*format <= '9'))
                {
                    std::streamsize precision = 0;

                    while ((*format >= '0')
                           && (*format <= '9'))
                    {
                        precision = (precision * 10)
                            + (*format - '0');
                        format++;
                    }

                    flags.stream.precision(precision);
                }
                else
                {
                    flags.stream.precision(10);
                }
            }
            else
            {
                flags.stream.precision(10);
            }

            // Skip any length specifiers because
            // we really don't care about them since
            // we're using typesafe C++

            switch (*format)
            {
            case 'j':
            case 'z':
            case 't':
            case 'L':
                format++;
                break;

            case 'h':
            case 'l':
                // Always accept this one - also accept
                // a second copy of the same char

                if (format[1] == format[0])
                    format++;
                format++;
                break;
            }

            // Finally, we expect the actual format specifier -
            // which we also skip if not zero

            char format_char = *format;
            if (format_char != 0)
                ++format;

            if ((format_char >= 'A')
                && (format_char <= 'Z'))
            {
                stream_flags |= std::ios_base::uppercase;
            }

            switch (format_char)
            {
            case 'd':
            case 'i':
            case 'u':
                // Defaults are fine
                break;

            case 'o':
                stream_flags = (stream_flags & ~std::ios_base::basefield) | std::ios_base::oct;
                break;

            case 'x':
            case 'X':
                stream_flags = (stream_flags & ~std::ios_base::basefield) | std::ios_base::hex;
                break;

            case 'f':
            case 'F':
                stream_flags = (stream_flags & ~std::ios_base::floatfield) | std::ios_base::fixed;
                break;

            case 'e':
            case 'E':
                stream_flags = (stream_flags & ~std::ios_base::floatfield) | std::ios_base::scientific;
                break;

            case 'g':
            case 'G':
                stream_flags = (stream_flags & ~std::ios_base::floatfield);
                break;

            case 'a':
            case 'A':
#ifdef WIN32
                stream_flags = (stream_flags & ~std::ios_base::floatfield) | std::ios_base::hexfloat;
#else
                // TODO - gcc doesn't have hexfloat
                stream_flags = (stream_flags & ~std::ios_base::floatfield);
#endif
                break;

            case 'c':
                flags.is_char = true;
                break;

            case 's':
            case 'p':
                // Default is find
                break;

            default:
                // Invalid format character
                break;
            }

            // Save the flags to the stream

            flags.stream.flags(stream_flags);

            return true;
        }

        void FormatDummyRest(const char *format, FormatFlags &flags)
        {
            while (FormatFindNext(format, flags))
            {
                flags.stream << "[**MISSSING**]";
            }
        }

    }

    void Format(std::ostream &stream, const char *format)
    {
        details::FormatFlags flags(stream);

        details::FormatDummyRest(format, flags);
    }

}