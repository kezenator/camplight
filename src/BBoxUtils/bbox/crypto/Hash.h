/**
 * @file
 *
 * Header file for the bbox::crypto::Hash class.
 */

#ifndef __BBOX__CRYPTO__HASH_H__
#define __BBOX__CRYPTO__HASH_H__

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace bbox {
    namespace crypto {

        /**
         * A hash value.
         */
        class Hash
        {
        public:
            Hash()
                : m_value()
            {
            }

            explicit Hash(const std::vector<uint8_t> &value)
                : m_value(value)
            {
            }

            explicit Hash(std::vector<uint8_t> &&value)
                : m_value(std::move(value))
            {
            }

            Hash(const void *data, size_t length)
                : m_value(static_cast<const uint8_t *>(data), static_cast<const uint8_t *>(data) + length)
            {
            }

            Hash(const Hash &other)
                : m_value(other.m_value)
            {
            }

            Hash(Hash &&other)
                : m_value(std::move(other.m_value))
            {
            }

            ~Hash()
            {
            }

            Hash &operator =(const Hash &other)
            {
                m_value = other.m_value;
                return *this;
            }

            Hash &operator =(Hash &&other)
            {
                m_value = std::move(other.m_value);
                return *this;
            }

            bool operator ==(const Hash &other) const { return m_value == other.m_value; }
            bool operator !=(const Hash &other) const { return m_value != other.m_value; }

            std::string ToString() const;
            std::string ToBase64String() const;

            const std::vector<uint8_t> &ToVectorUint8() const { return m_value; }

        private:
            std::vector<uint8_t> m_value;
        };


    } // namespace bbox::crypto
} // namespace bbox

#endif // __BBOX__CRYPTO__HASH_H__
