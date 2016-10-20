/**
 * @file
 *
 * Implementation file for the bbox::Base64 class.
 */

#include <bbox/Base64.h>

// LICENCE
// This code was retrieved and edited from
// http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
// on 28th March 2014. It is believed
// to be in the public domain.

namespace {

    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


    inline bool is_base64(char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

} // anonymous namespace

namespace bbox {

    std::string Base64::Encode(const void *data, size_t length)
    {
        std::string ret;

        const uint8_t *buf = static_cast<const uint8_t *>(data);
        int i = 0;
        int j = 0;
        uint8_t char_array_3[3];
        uint8_t char_array_4[4];

        while (length--)
        {
            char_array_3[i++] = *(buf++);
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    Error Base64::Decode(const std::string &encoded, std::vector<uint8_t> &decoded_data)
    {
        decoded_data.clear();
        decoded_data.reserve(encoded.size());

        size_t in_len = encoded.size();
        size_t i = 0;
        size_t j = 0;
        size_t in_ = 0;
        uint8_t char_array_4[4], char_array_3[3];

        while (in_len-- && (encoded[in_] != '=') && is_base64(encoded[in_]))
        {
            char_array_4[i++] = encoded[in_]; in_++;
            if (i == 4)
            {
                for (i = 0; i <4; i++)
                    char_array_4[i] = uint8_t(base64_chars.find(char_array_4[i]));

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    decoded_data.push_back(char_array_3[i]);
                i = 0;
            }
        }

        // Skip trailing equals signs
        while ((in_ < encoded.size())
            && (encoded[in_] == '='))
        {
            ++in_;
        }

        if ((in_ < encoded.size())
            && !is_base64(encoded[in_]))
        {
            return std::errc::invalid_argument;
        }

        if (i)
        {
            for (j = i; j <4; j++)
                char_array_4[j] = 0;

            for (j = 0; j <4; j++)
                char_array_4[j] = uint8_t(base64_chars.find(char_array_4[j]));

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) decoded_data.push_back(char_array_3[j]);
        }

        return Error();
    }

} // namespace bbox
