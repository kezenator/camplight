/**
 * @file
 *
 * Header file for the bbox::Uuid class.
 */

#ifndef __BBOX__UUID_H__
#define __BBOX__UUID_H__

#pragma once

#include <iosfwd>
#include <string>
#include <vector>
#include <initializer_list>

#include <boost/uuid/uuid.hpp>

namespace bbox {

    class Uuid
    {
        friend void swap(Uuid &u1, Uuid &u2);
        friend size_t hash_value(const Uuid &uuid);

    public:

        static const size_t SIZE = 16;

        Uuid()
            : m_uuid({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })
        {
        }

        Uuid(const std::initializer_list<uint8_t> &init_list);

        Uuid(const Uuid &other)
            : m_uuid(other.m_uuid)
        {
        }

        explicit Uuid(const boost::uuids::uuid &uuid)
            : m_uuid(uuid)       
        {
        }

        explicit Uuid(const std::string &str);

        Uuid &operator =(const Uuid &other)
        {
            if (this != &other)
            {
                m_uuid = other.m_uuid;
            }
            return *this;
        }

        std::vector<uint8_t> GetBytes() const
        {
            return std::vector<uint8_t>(m_uuid.begin(), m_uuid.end());
        }

        std::string ToString() const;
        std::string ToBracelessString() const;
        std::string ToUrn() const;

        static bool FromString(const std::string &str, Uuid &uuid);

        void GenerateNamed(const Uuid &namespace_uuid, const std::string &name);
        void GenerateRandom();

        static Uuid NewNamed(const Uuid &namespace_uuid, const std::string &name)
        {
            Uuid result;
            result.GenerateNamed(namespace_uuid, name);
            return result;
        }

        static Uuid NewRandom()
        {
            Uuid result;
            result.GenerateRandom();
            return result;
        }

        bool operator ==(const Uuid &other) const { return m_uuid == other.m_uuid; }
        bool operator !=(const Uuid &other) const { return m_uuid != other.m_uuid; }
        bool operator  <(const Uuid &other) const { return m_uuid  < other.m_uuid; }
        bool operator <=(const Uuid &other) const { return m_uuid <= other.m_uuid; }
        bool operator  >(const Uuid &other) const { return m_uuid  > other.m_uuid; }
        bool operator >=(const Uuid &other) const { return m_uuid >= other.m_uuid; }

    private:
        boost::uuids::uuid m_uuid;
    };

    inline std::ostream &operator <<(std::ostream &stream, const Uuid &guid)
    {
        return stream << guid.ToString();
    }

    inline void swap(Uuid &u1, Uuid &u2)
    {
        u1.m_uuid.swap(u2.m_uuid);
    }

    inline size_t hash_value(const Uuid &uuid)
    {
        return hash_value(uuid.m_uuid);
    }

} // namespace bbox

#endif // __BBOX__UUID_H__
