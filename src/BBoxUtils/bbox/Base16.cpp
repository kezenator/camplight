/**
 * @file
 *
 * Implementation file for the bbox::Base16 class.
 */

#include <bbox/Base16.h>
#include <cctype>

namespace {

    const std::string base16_chars =
        "0123456789ABCDEF";


    inline bool is_base16(char c)
    {
        return isxdigit(c) != 0;
    }

    inline uint8_t base16_decode(char c)
    {
        if ((c >= 'A') && (c <= 'F'))
            return c + 10 - 'A';
        else if ((c >= 'a') && (c <= 'f'))
            return c + 10 - 'a';
        else
            return c - '0';
    }

} // anonymous namespace

namespace bbox {

    std::string Base16::Encode(const void *data, size_t length)
    {
        const uint8_t *buf = static_cast<const uint8_t *>(data);

        std::string ret(2 * length, '0');

        for (size_t buf_idx = 0; buf_idx < length; ++buf_idx)
        {
            size_t str_idx = 2 * buf_idx;

            ret[str_idx + 0] = base16_chars[(buf[buf_idx] >> 4) & 0x0F];
            ret[str_idx + 1] = base16_chars[buf[buf_idx] & 0x0F];
        }

        return ret;
    }

    Error Base16::Decode(const std::string &encoded, std::vector<uint8_t> &decoded_data)
    {
        decoded_data.clear();

        size_t in_len = encoded.size();

        if ((in_len & 1) != 0)
        {
            // String is not even length
            return std::errc::invalid_argument;
        }

        decoded_data.resize(in_len / 2);

        for (size_t enc_idx = 0; enc_idx < in_len; enc_idx += 2)
        {
            size_t dec_idx = enc_idx / 2;

            char ch1 = encoded[enc_idx + 0];
            char ch2 = encoded[enc_idx + 1];

            if (!is_base16(ch1) || !is_base16(ch2))
                return std::errc::invalid_argument;

            uint8_t decode1 = base16_decode(ch1);
            uint8_t decode2 = base16_decode(ch2);

            decoded_data[dec_idx] = (decode1 << 4) | decode2;
        }

        return Error();
    }

} // namespace bbox
