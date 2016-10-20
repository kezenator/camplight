/**
* @file
*
* Implementation file for the bbox::enc::FromBinary class.
*
*/

#include <bbox/enc/FromBinary.h>
#include <bbox/Format.h>
#include <cstring>

namespace bbox
{
    namespace enc
    {

        FromBinary::FromBinary(const void *data, size_t data_size)
            : m_Data(static_cast<const uint8_t *>(data))
            , m_DataLength(data_size)
            , m_ReadPtr(m_Data)
            , m_HasError(false)
            , m_ErrorStr()
        {
        }

        FromBinary::~FromBinary()
        {
        }

        void FromBinary::EnsureAllBytesRead()
        {
            if (!m_HasError
                && (GetBytesRemaining() != 0))
            {
                SetError(bbox::Format("Unexpected %d bytes remaining at end", GetBytesRemaining()));
            }
        }

        void FromBinary::SetError(const std::string &err)
        {
            if (!m_HasError)
            {
                m_HasError = true;
                m_ErrorStr = err;
            }
        }

        void FromBinary::SetError(std::string &&err)
        {
            if (!m_HasError)
            {
                m_HasError = true;
                m_ErrorStr = std::move(err);
            }
        }

        void FromBinary::ReadData(void *data, size_t size)
        {
            if (PrivateCanRead(size))
            {
                memcpy(data, m_ReadPtr, size);
                m_ReadPtr += size;
            }
        }

        std::string FromBinary::ReadString()
        {
            std::string result;

            uint64_t size = ReadSizeT();
                
            if (!m_HasError)
            {
                if (size > GetBytesRemaining())
                {
                    SetError("String value too long");
                }
                else if (size != 0)
                {
                    result.resize(size_t(size));
                    memcpy(&result[0], m_ReadPtr, size_t(size));
                    m_ReadPtr += size;
                }
            }

            return result;
        }

        std::vector<uint8_t> FromBinary::ReadByteVector()
        {
            std::vector<uint8_t> result;

            uint64_t size = ReadSizeT();

            if (!m_HasError)
            {
                if (size > GetBytesRemaining())
                {
                    SetError("Byte array value too long");
                }
                else if (size != 0)
                {
                    result.resize(size_t(size));
                    memcpy(&result[0], m_ReadPtr, size_t(size));
                    m_ReadPtr += size;
                }
            }

            return result;
        }

        size_t FromBinary::ReadSizeT()
        {
            uint64_t size = ReadUint64();

            if ((size < std::numeric_limits<size_t>::min())
                || (size > std::numeric_limits<size_t>::max()))
            {
                SetError("size_t value out of range");
                return 0;
            }

            return size_t(size);
        }

    } // namespace bbox::enc
} // namespace bbox
