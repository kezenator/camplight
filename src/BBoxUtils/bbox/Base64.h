/**
 * @file
 *
 * Header file for the bbox::Base64 class.
 */

#ifndef __BBOX__BASE_64_H__
#define __BBOX__BASE_64_H__

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <bbox/Error.h>

namespace bbox {

    /**
     * Provides access to Base64 coding functions.
     */
    class Base64
    {
        // Instances are not allowed
        Base64() = delete;
        Base64(const Base64 &) = delete;
        void operator =(const Base64 &) = delete;

    public:

        /**
         * Encodes data to a base-64 string.
         */
        static std::string Encode(const void *data, size_t length);

        /**
         * Encodes data to a base-64 string.
         */
        static std::string Encode(const std::vector<uint8_t> &data)
        {
            return Encode(data.empty() ? nullptr : &data[0], data.size());
        }

        /**
         * Encodes data to a base-64 string.
         */
        static std::string Encode(const std::string &data)
        {
            static_assert(sizeof(data[0]) == 1, "std::string does not contain characters");

            return Encode(data.empty() ? nullptr : &data[0], data.size());
        }

        /**
         * Decodes a base-64 string to a new vector.
         */
        static Error Decode(const std::string &encoded, std::vector<uint8_t> &decoded_data);
    };

}

#endif // __BBOX__BASE_64_H__
