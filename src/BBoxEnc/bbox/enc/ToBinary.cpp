/**
* @file
*
* Implementation file for the bbox::enc::ToBinary class.
*
*/

#include <bbox/enc/ToBinary.h>
#include <cstring>

namespace bbox
{
    namespace enc
    {

        ToBinary::ToBinary()
            : m_Buffer(new uint8_t [1024])
            , m_BufferLength(1024)
            , m_WritePtr(m_Buffer)
            , m_AutoAllocated(true)
            , m_HasError(false)
            , m_ErrorStr()
        {
        }

        ToBinary::ToBinary(size_t expected_size)
            : m_Buffer(new uint8_t[expected_size])
            , m_BufferLength(expected_size)
            , m_WritePtr(m_Buffer)
            , m_AutoAllocated(true)
            , m_HasError(false)
            , m_ErrorStr()
        {
        }

        ToBinary::ToBinary(void *buffer, size_t buffer_size)
            : m_Buffer(static_cast<uint8_t *>(buffer))
            , m_BufferLength(buffer_size)
            , m_WritePtr(m_Buffer)
            , m_AutoAllocated(false)
            , m_HasError(false)
            , m_ErrorStr()
        {
        }

        ToBinary::~ToBinary()
        {
            if (m_AutoAllocated)
            {
                delete[] m_Buffer;
            }
        }

        std::vector<uint8_t> ToBinary::ExtractData()
        {
            std::vector<uint8_t> result;

            if (!m_HasError)
            {
                size_t bytes_written = GetBytesWritten();

                if (bytes_written != 0)
                {
                    result.resize(bytes_written);
                    memcpy(&result[0], m_Buffer, bytes_written);
                }
            }

            return result;
        }

        void ToBinary::SetError(const std::string &err)
        {
            if (!m_HasError)
            {
                m_HasError = true;
                m_ErrorStr = err;
            }
        }

        void ToBinary::SetError(std::string &&err)
        {
            if (!m_HasError)
            {
                m_HasError = true;
                m_ErrorStr = std::move(err);
            }
        }

        void ToBinary::WriteData(const void *data, size_t length)
        {
            if (PrivateTestForWrite(length))
            {
                memcpy(m_WritePtr, data, length);
                m_WritePtr += length;
            }
        }

        void ToBinary::WriteString(const std::string &str)
        {
            WriteSizeT(str.size());

            if (!str.empty())
            {
                if (PrivateTestForWrite(str.size()))
                {
                    memcpy(m_WritePtr, &str[0], str.size());
                    m_WritePtr += str.size();
                }
            }
        }

        void ToBinary::WriteByteVector(const std::vector<uint8_t> &vec)
        {
            WriteSizeT(vec.size());

            if (!vec.empty())
            {
                if (PrivateTestForWrite(vec.size()))
                {
                    memcpy(m_WritePtr, &vec[0], vec.size());
                    m_WritePtr += vec.size();
                }
            }
        }

		void ToBinary::WriteMsgAnyPtr(const MsgAnyPtr &val)
		{
			if (!val)
			{
				WriteString(std::string());
			}
			else
			{
				WriteString(val.GetType().GetName());
				if (val)
				{
					val.m_value->EncodeContents(*this);
				}
			}
		}

        bool ToBinary::PrivateTestForWrite(size_t size)
        {
            if (m_HasError)
            {
                // Already has an error raised
                return false;
            }

            size_t written = m_WritePtr - m_Buffer;
            size_t remaining = m_BufferLength - written;

            if (remaining >= size)
            {
                // There is already enought buffer space
                return true;
            }

            if (!m_AutoAllocated)
            {
                // Can't allocate more space for manually managed buffer
                SetError("Exceeds provided buffer size");
                return false;
            }

            size_t new_buffer_length = 2 * m_BufferLength;
            if (new_buffer_length < m_BufferLength)
            {
                // Too much data written
                SetError("Exceeds maximum size");
                return false;
            }

            uint8_t *new_buffer = new uint8_t[new_buffer_length];

            memcpy(new_buffer, m_Buffer, written);

            uint8_t *old_buffer = m_Buffer;

            m_Buffer = new_buffer;
            m_BufferLength = new_buffer_length;
            m_WritePtr = m_Buffer + written;

            delete[] old_buffer;

            return true;
        }

    } // namespace bbox::enc
} // namespace bbox
