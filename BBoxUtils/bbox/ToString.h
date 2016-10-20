/**
 * Header file for the bbox::ToString template function.
 */

#ifndef __BBOX__TO_STRING_H__
#define __BBOX__TO_STRING_H__

#include <string>
#include <type_traits>
#include <sstream>

namespace bbox
{
    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Strings
    //

    inline std::string ToString(const char *t)
    {
        return t;
    }

    inline const std::string &ToString(const std::string &t)
    {
        return t;
    }

    inline std::string &&ToString(std::string &&t)
    {
        return std::move(t);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Boolean
    //

    inline std::string ToString(bool value)
    {
        return value ? "true" : "false";
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Integers
    //

    template <typename Type>
    inline auto ToString(Type val) -> typename std::enable_if<std::is_integral<Type>::value, std::string>::type
    {
        std::stringstream stream;
        stream << val;
        return stream.str();
    }

    // char and unsigned char need separate
    // cases otherwise they get treated as characters

    inline std::string ToString(signed char val)
    {
        std::stringstream stream;
        stream << (int)val;
        return stream.str();
    }

    inline std::string ToString(unsigned char val)
    {
        std::stringstream stream;
        stream << (unsigned int)val;
        return stream.str();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Other values - for now just assume they have a ToString method.
    //

    template <typename Type>
    inline auto ToString(const Type &val) -> decltype(val.ToString())
    {
        return val.ToString();
    }

} // namespace bbox

#endif // __BBOX__TO_STRING_H__
