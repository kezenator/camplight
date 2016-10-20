/**
* @file
*
* Header file for the bbox::TypeInfo class.
*/

#ifndef __BBOX__TYPE_INFO_H__
#define __BBOX__TYPE_INFO_H__

#pragma once

#include <boost/type_index.hpp>
#include <string>
#include <new>

namespace bbox {

    /**
     * A simple wrapper around std::type_info
     * that turns it into a value type - e.g.
     * that can directly be used in std::map and
     * std::set without needing custom comparitors.
     */
    class TypeInfo
    {
    public:

        /**
         * Gets a TypeInfo value for a type.
         *
         * Use as:
         * @code
         * TypeInfo blah = TypeInfo::Of<MyType>();
         * @endcode
         */
        template <typename Type>
        static TypeInfo Of()
        {
            return typeid(Type);
        }

        TypeInfo()
            : m_type_index(typeid(void))
        {
        }

        TypeInfo(const std::type_info &type)
            : m_type_index(type)
        {
        }

        TypeInfo(const boost::typeindex::type_index &type)
            : m_type_index(type)
        {
        }

        TypeInfo(const TypeInfo &other)
            : m_type_index(other.m_type_index)
        {
        }

        TypeInfo &operator =(const TypeInfo &other)
        {
            m_type_index = other.m_type_index;
            return *this;
        }

        TypeInfo &operator =(const std::type_info &type)
        {
            m_type_index = type;
            return *this;
        }

        TypeInfo &operator =(const boost::typeindex::type_index &type)
        {
            m_type_index = type.type_info();
            return *this;
        }

        const char *raw_name() const
        {
            return m_type_index.raw_name();
        }

        std::string pretty_name() const
        {
            return m_type_index.pretty_name();
        }

        size_t hash_code() const
        {
            return m_type_index.hash_code();
        }

        bool operator ==(const TypeInfo &other) const { return m_type_index == other.m_type_index; }
        bool operator !=(const TypeInfo &other) const { return m_type_index != other.m_type_index; }
        bool operator  <(const TypeInfo &other) const { return m_type_index  < other.m_type_index; }
        bool operator <=(const TypeInfo &other) const { return m_type_index <= other.m_type_index; }
        bool operator  >(const TypeInfo &other) const { return m_type_index  > other.m_type_index; }
        bool operator >=(const TypeInfo &other) const { return m_type_index >= other.m_type_index; }

    private:
        boost::typeindex::type_index m_type_index;
    };

} // namespace bbox

#endif // __BBOX__TYPE_INFO_H__
