/**
 * @file
 *
 * Header file for the bbox::FromString functions.
 */

#ifndef __BBOX__FROM_STRING_H__
#define __BBOX__FROM_STRING_H__

#pragma once

#include <string>
#include <stdexcept>
#include <type_traits>
#include <limits>

namespace bbox {

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Strings
    //

    inline bool FromString(const std::string &str, std::string &val)
    {
        val = str;
        return true;
    }

    inline bool FromString(std::string &&str, std::string &val)
    {
        val = std::move(str);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Boolean
    //

    inline bool FromString(const std::string &str, bool &val)
    {
        // TODO - handle more cases - e.g. case insensitive, yes/no, 1/0 etc.

        if (str == "true")
        {
            val = true;
            return true;
        }
        else if (str == "false")
        {
            val = false;
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Integers
    //

    template <typename Type>
    inline auto FromString(const std::string &str, Type &val) -> typename std::enable_if<std::is_integral<Type>::value
                                                                                         && std::is_signed<Type>::value, bool>::type
    {
        using CoreIntType = typename std::remove_reference<Type>::type;
        using ConvertType = long long;

        static_assert(std::numeric_limits<CoreIntType>::min() >= std::numeric_limits<ConvertType>::min(), "Convert type doesn't hold int type");
        static_assert(std::numeric_limits<CoreIntType>::max() <= std::numeric_limits<ConvertType>::max(), "Convert type doesn't hold int type");

        try
        {
            size_t idx;

            ConvertType large_val = std::stoll(str, &idx);

            if ((idx == str.size())
                && (large_val >= std::numeric_limits<CoreIntType>::min())
                && (large_val <= std::numeric_limits<CoreIntType>::max()))
            {
                val = CoreIntType(large_val);
                return true;
            }
            return false;
        }
        catch (std::invalid_argument &)
        {
            return false;
        }
        catch (std::out_of_range &)
        {
            return false;
        }
    }

    template <typename Type>
    inline auto FromString(const std::string &str, Type &val) -> typename std::enable_if<std::is_integral<Type>::value
                                                                                         && std::is_unsigned<Type>::value, bool>::type
    {
        using CoreIntType = typename std::remove_reference<Type>::type;
        using ConvertType = unsigned long long;

        static_assert(std::numeric_limits<CoreIntType>::min() >= std::numeric_limits<ConvertType>::min(), "Convert type doesn't hold int type");
        static_assert(std::numeric_limits<CoreIntType>::max() <= std::numeric_limits<ConvertType>::max(), "Convert type doesn't hold int type");

        try
        {
            size_t idx;

            ConvertType large_val = std::stoull(str, &idx);

            if ((idx == str.size())
                && (large_val >= std::numeric_limits<CoreIntType>::min())
                && (large_val <= std::numeric_limits<CoreIntType>::max()))
            {
                val = CoreIntType(large_val);
                return true;
            }
            return false;
        }
        catch (std::invalid_argument &)
        {
            return false;
        }
        catch (std::out_of_range &)
        {
            return false;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Custom types - if the type has a static FromString method
    //

    template <typename Type>
    inline auto FromString(const std::string &str, Type &val)
        -> typename std::enable_if<!std::is_integral<Type>::value, bool>::type
    {
        using CoreType = typename std::remove_reference<Type>::type;

        return CoreType::FromString(str, val);
    }

} // namespace bbox

#endif // __BBOX__FROM_STRING_H__
