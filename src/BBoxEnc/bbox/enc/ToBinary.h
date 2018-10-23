/**
 * @file
 *
 * Header file for the bbox::enc::ToBinary class.
 *
 */

#ifndef __BBOX__ENC__TO_ENCARY_H__
#define __BBOX__ENC__TO_ENCARY_H__

#include <cstdint>
#include <string>
#include <vector>

#include <bbox/enc/details/MethodDetection.h>
#include <bbox/enc/details/MarshalActions.h>

namespace bbox
{
    namespace enc
    {

        /**
            * A class that assists marshaling data to a binary format
            * to either a user-provided or automatically allocated buffer.
            */
        class ToBinary
        {
        private:
            ToBinary(const ToBinary &) = delete;
            void operator =(const ToBinary &) = delete;

        public:
            /**
                * Constructs to write to an automatically allocated
                * resizing buffer.
                */
            ToBinary();

            /**
                * Constructs to write to an automatically allocated
                * resizing buffer with a hint to the size expected.
                */
            explicit ToBinary(size_t expected_size);

            /**
                * Constructs to write to the specified buffer.
                * All memory management of the buffer is the responsibitlity
                * of the caller. The buffer must remain valid for the life
                * of the ToBinary object.
                */
            ToBinary(void *buffer, size_t buffer_size);

            /**
                * Destructor.
                */
            ~ToBinary();

            /**
                * Gets the number of bytes written so far.
                */
            size_t GetBytesWritten() const { return m_WritePtr - m_Buffer; }

            std::vector<uint8_t> ExtractData();

            /**
                * Determines if an error has been raised.
                */
            bool HasError() const { return m_HasError; }

            /**
                * Gets the error string that has been raised.
                */
            const std::string &GetErrorString() const { return m_ErrorStr; }

            /**
                * Sets an error.
                */
            void SetError(const std::string &err);

            /**
            * Sets an error.
            */
            void SetError(std::string &&err);

            /**
                * Writes a 8-bit unsigned int.
                */
            void WriteUint8(uint8_t val)
            {
                if (PrivateTestForWrite(1))
                {
                    m_WritePtr[0] = val;
                    m_WritePtr += 1;
                }
            }

            /**
            * Writes a 16-bit unsigned int.
            */
            void WriteUint16(uint16_t val)
            {
                if (PrivateTestForWrite(2))
                {
                    m_WritePtr[0] = uint8_t(val);
                    m_WritePtr[1] = uint8_t(val >> 8);
                    m_WritePtr += 2;
                }
            }

            /**
            * Writes a 16-bit unsigned int in big-endian format.
            */
            void WriteUint16BE(uint16_t val)
            {
                if (PrivateTestForWrite(2))
                {
                    m_WritePtr[0] = uint8_t(val >> 8);
                    m_WritePtr[1] = uint8_t(val);
                    m_WritePtr += 2;
                }
            }

            static inline void UncheckedEncodeUint32(void *buffer, uint32_t val)
            {
                uint8_t *dst = static_cast<uint8_t *>(buffer);

                dst[0] = uint8_t(val);
                dst[1] = uint8_t(val >> 8);
                dst[2] = uint8_t(val >> 16);
                dst[3] = uint8_t(val >> 24);
            }

            /**
            * Writes a 32-bit unsigned int.
            */
            void WriteUint32(uint32_t val)
            {
                if (PrivateTestForWrite(4))
                {
                    UncheckedEncodeUint32(m_WritePtr, val);
                    m_WritePtr += 4;
                }
            }

            static inline void UncheckedEncodeUint64(void *buffer, uint64_t val)
            {
                uint8_t *dst = static_cast<uint8_t *>(buffer);

                dst[0] = uint8_t(val);
                dst[1] = uint8_t(val >> 8);
                dst[2] = uint8_t(val >> 16);
                dst[3] = uint8_t(val >> 24);
                dst[4] = uint8_t(val >> 32);
                dst[5] = uint8_t(val >> 40);
                dst[6] = uint8_t(val >> 48);
                dst[7] = uint8_t(val >> 56);
            }

            /**
            * Writes a 64-bit unsigned int.
            */
            void WriteUint64(uint64_t val)
            {
                if (PrivateTestForWrite(8))
                {
                    UncheckedEncodeUint64(m_WritePtr, val);
                    m_WritePtr += 8;
                }
            }

            void WriteSizeT(size_t val)
            {
                WriteUint64(uint64_t(val));
            }

            /**
                * Writes a block of data to the stream.
                */
            void WriteData(const void *data, size_t length);

            void WriteString(const std::string &str);
            void WriteByteVector(const std::vector<uint8_t> &vec);
            void WriteBool(const bool &val) { WriteUint8(val ? 0x01 : 0x00); }

			void WriteMsgAnyPtr(const MsgAnyPtr &val);

            void WriteIntByType(const uint8_t &val) { WriteUint8(val); }
            void WriteIntByType(const uint16_t &val) { WriteUint16(val); }
            void WriteIntByType(const uint32_t &val) { WriteUint32(val); }
            void WriteIntByType(const uint64_t &val) { WriteUint64(val); }

            void WriteIntByType(const int8_t &val) { WriteUint8(uint8_t(val)); }
            void WriteIntByType(const int16_t &val) { WriteUint16(uint16_t(val)); }
            void WriteIntByType(const int32_t &val) { WriteUint32(uint32_t(val)); }
            void WriteIntByType(const int64_t &val) { WriteUint64(uint64_t(val)); }

            template <typename Type>
            void Write(const Type &val)
            {
                using Action = details::ToBinaryAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                Action::Impl(*this, val);
            }

        private:

            bool PrivateTestForWrite(size_t size);

            uint8_t *m_Buffer;
            size_t m_BufferLength;
            uint8_t *m_WritePtr;
            bool m_AutoAllocated;
            bool m_HasError;
            std::string m_ErrorStr;
        };

    } // namespace bbox::enc
} // namespace bbox

#ifndef __BBOX__ENC__MSG_PTR_H__
#include <bbox/enc/details/ToBinaryAction.h>
#endif // not __BBOX__ENC__MSG_PTR_H__

#endif // __BBOX__ENC__TO_ENCARY_H__
