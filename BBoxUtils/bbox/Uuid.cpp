/**
 * @file
 *
 * Implementation file for the bbox::Uuid class.
 */

#include <bbox/Uuid.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#ifdef _WIN32
#include <Windows.h>
#include <Objbase.h>
#else // not _WIN32
#include <uuid/uuid.h>
#endif // not _WIN32

namespace bbox {

    Uuid::Uuid(const std::initializer_list<uint8_t> &init_list)
    {
        BBOX_ASSERT(init_list.size() == SIZE);
        BBOX_ASSERT(boost::uuids::uuid::static_size() == SIZE);

        memcpy(m_uuid.begin(), init_list.begin(), SIZE);
    }

    Uuid::Uuid(const std::string &str)
        : m_uuid()
    {
        bool converted = FromString(str, *this);
        BBOX_ASSERT(converted);
    }

    std::string Uuid::ToString() const
    {
        const uint8_t *data = m_uuid.begin();

        return Format("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                      data[0],  data[1],  data[2],  data[3],
                      data[4],  data[5],  data[6],  data[7],
                      data[8],  data[9],  data[10], data[11],
                      data[12], data[13], data[14], data[15]);
    }

    std::string Uuid::ToBracelessString() const
    {
        const uint8_t *data = m_uuid.begin();

        return Format("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                      data[0], data[1], data[2], data[3],
                      data[4], data[5], data[6], data[7],
                      data[8], data[9], data[10], data[11],
                      data[12], data[13], data[14], data[15]);
    }

    std::string Uuid::ToUrn() const
    {
        const uint8_t *data = m_uuid.begin();

        return Format("urn:uuid:%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                      data[0], data[1], data[2], data[3],
                      data[4], data[5], data[6], data[7],
                      data[8], data[9], data[10], data[11],
                      data[12], data[13], data[14], data[15]);
    }

    bool Uuid::FromString(const std::string &str, Uuid &uuid)
    {
        size_t index = 0;

        // First, accept "urn:uuid:"

        if ((str.size() > 9)
            && (str.compare(0, 9, "urn:uuid:", 9) == 0))
        {
            index = 9;
        }

        // Next, accept a opening brace

        bool has_braces = false;

        if ((str.size() > index)
            && (str[index] == '{'))
        {
            ++index;
            has_braces = true;
        }

        // Now, expect 16 hex digits, with
        // possible dashes at defined positions

        bool has_dashes = false;

        for (size_t num = 0; num < SIZE; ++num)
        {
            switch (num)
            {
            case 4:
                if ((str.size() > index)
                    && (str[index] == '-'))
                {
                    ++index;
                    has_dashes = true;
                }
                break;

            case 6:
            case 8:
            case 10:
                if (has_dashes)
                {
                    if ((str.size() <= index)
                        && (str[index] != '-'))
                    {
                        return false;
                    }
                    ++index;
                }
                break;
            }

            if ((index + 2) > str.size())
            {
                // Not enough characters
                return false;
            }

            struct from_hex
            {
                static bool convert(char ch, uint8_t &val)
                {
                    if ((ch >= '0') && (ch <= '9'))
                    {
                        val = uint8_t(ch - '0');
                        return true;
                    }
                    else if ((ch >= 'A') && (ch <= 'F'))
                    {
                        val = uint8_t(ch - 'A' + 10);
                        return true;
                    }
                    else if ((ch >= 'a') && (ch <= 'f'))
                    {
                        val = uint8_t(ch - 'a' + 10);
                        return true;
                    }
                    return false;
                }
            };

            uint8_t high_nibble, low_nibble;

            if (!from_hex::convert(str[index], high_nibble)
                || !from_hex::convert(str[index + 1], low_nibble))
            {
                // Not valid hex chars
                return false;
            }

            uuid.m_uuid.begin()[num] = (high_nibble << 4) | low_nibble;

            index += 2;
        }

        // Finally, a closing brace is required
        // if we had the start

        if (has_braces)
        {
            if ((str.size() <= index)
                || (str[index] != '}'))
            {
                return false;
            }
            ++index;
        }

        // If we're not at the end of the string
        // then there's something wrong

        if (index != str.size())
            return false;

        // OK - all good

        return true;
    }

    void Uuid::GenerateNamed(const Uuid &namespace_uuid, const std::string &name)
    {
        boost::uuids::name_generator gen(namespace_uuid.m_uuid);

        m_uuid = gen(name);
    }

    void Uuid::GenerateRandom()
    {
#ifdef _WIN32
        GUID guid;

        HRESULT co_result = CoCreateGuid(&guid);

        BBOX_ASSERT(co_result == S_OK);

        uint8_t *buf = m_uuid.begin();

        buf[0] = uint8_t(guid.Data1 >> 24);
        buf[1] = uint8_t(guid.Data1 >> 16);
        buf[2] = uint8_t(guid.Data1 >> 8);
        buf[3] = uint8_t(guid.Data1 >> 0);

        buf[4] = uint8_t(guid.Data2 >> 8);
        buf[5] = uint8_t(guid.Data2 >> 0);

        buf[6] = uint8_t(guid.Data3 >> 8);
        buf[7] = uint8_t(guid.Data3 >> 0);

        buf[8] = guid.Data4[0];
        buf[9] = guid.Data4[1];

        buf[10] = guid.Data4[2];
        buf[11] = guid.Data4[3];
        buf[12] = guid.Data4[4];
        buf[13] = guid.Data4[5];
        buf[14] = guid.Data4[6];
        buf[15] = guid.Data4[7];

        BBOX_ASSERT(m_uuid.variant() == boost::uuids::uuid::variant_rfc_4122);
        BBOX_ASSERT(m_uuid.version() == boost::uuids::uuid::version_random_number_based);

#else // not _WIN32

        uuid_t uuid_buf;
        uuid_generate_random(uuid_buf);

        uint8_t *dest_buf = m_uuid.begin();

        memcpy(dest_buf, uuid_buf, 16);

        BBOX_ASSERT(m_uuid.variant() == boost::uuids::uuid::variant_rfc_4122);
        BBOX_ASSERT(m_uuid.version() == boost::uuids::uuid::version_random_number_based);
#endif // not _WIN32
    }

} // namespace bbox
