/**
 * @file
 *
 * Header file for the bbox::Base16 class.
 */

#ifndef __BBOX__BASE_16_H__
#define __BBOX__BASE_16_H__

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <bbox/Error.h>

namespace bbox {

    /**
     * Provides access to Base16 coding functions.
     */
    class Base16
    {
        // Instances are not allowed
        Base16() = delete;
        Base16(const Base16 &) = delete;
        void operator =(const Base16 &) = delete;

    public:

        /**
         * Encodes data to a base-16 string.
         */
        static std::string Encode(const void *data, size_t length);

        /**
        * Encodes data to a base-16 string.
        */
        static std::string Encode(const std::vector<uint8_t> &data)
        {
            return Encode(data.empty() ? nullptr : &data[0], data.size());
        }

        /**
         * Decodes a base-16 string to a new vector.
         */
        static Error Decode(const std::string &encoded, std::vector<uint8_t> &decoded_data);
    };

}

#endif // __BBOX__BASE_16_H__
