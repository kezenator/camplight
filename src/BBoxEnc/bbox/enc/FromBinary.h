/**
* @file
*
* Header file for the bbox::enc::FromBinary class.
*
*/

#ifndef __BBOX__ENC__FROM_ENCARY_H__
#define __BBOX__ENC__FROM_ENCARY_H__

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
         * A class that assists de-marshaling data from a binary format.
         */
        class FromBinary
        {
        private:
            FromBinary() = delete;
            FromBinary(const FromBinary &) = delete;
            void operator =(const FromBinary &) = delete;

        public:

            /**
             * Constructs to read from the specified buffer.
             * All memory management of the buffer is the responsibitlity
             * of the caller. The buffer must remain valid for the life
             * of the FromBinary object.
             */
            FromBinary(const void *data, size_t data_size);

            /**
             * Destructor.
             */
            ~FromBinary();

            /**
             * Gets the number of bytes read so far.
             */
            size_t GetBytesRead() const { return m_ReadPtr - m_Data; }

            /**
             * Gets the number of bytes remaining to be read.
             */
            size_t GetBytesRemaining() const { return m_DataLength - GetBytesRead(); }

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
             * Ensures that all bytes are read correctly
             * and no bytes are remaining.
             */
            void EnsureAllBytesRead();

            void ReadData(void *data, size_t size);

            std::string ReadString();
            std::vector<uint8_t> ReadByteVector();

            inline uint8_t ReadUint8()
            {
                uint8_t result = 0;

                if (PrivateCanRead(1))
                {
                    result = m_ReadPtr[0];
                    m_ReadPtr += 1;
                }

                return result;
            }

            inline uint16_t ReadUint16()
            {
                uint16_t result = 0;

                if (PrivateCanRead(2))
                {
                    result = m_ReadPtr[0]
                        | (uint64_t(m_ReadPtr[1]) << 8);
                    m_ReadPtr += 2;
                }

                return result;
            }

            static inline uint32_t UncheckedDecodeUint32(const void *data)
            {
                const uint8_t *src = static_cast<const uint8_t *>(data);

                return src[0]
                    | (uint32_t(src[1]) << 8)
                    | (uint32_t(src[2]) << 16)
                    | (uint32_t(src[3]) << 24);
            }

            inline uint32_t ReadUint32()
            {
                uint32_t result = 0;

                if (PrivateCanRead(4))
                {
                    result = UncheckedDecodeUint32(m_ReadPtr);
                    m_ReadPtr += 4;
                }

                return result;
            }

            inline uint64_t ReadUint64()
            {
                uint64_t result = 0;

                if (PrivateCanRead(8))
                {
                    result = m_ReadPtr[0]
                        | (uint64_t(m_ReadPtr[1]) << 8)
                        | (uint64_t(m_ReadPtr[2]) << 16)
                        | (uint64_t(m_ReadPtr[3]) << 24)
                        | (uint64_t(m_ReadPtr[4]) << 32)
                        | (uint64_t(m_ReadPtr[5]) << 40)
                        | (uint64_t(m_ReadPtr[6]) << 48)
                        | (uint64_t(m_ReadPtr[7]) << 56);
                    m_ReadPtr += 8;
                }

                return result;
            }

            size_t ReadSizeT();

            bool ReadBool()
            {
                return ReadUint8() != 0;
            }

            void ReadIntByType(uint8_t &val) { val = ReadUint8(); }
            void ReadIntByType(uint16_t &val) { val = ReadUint16(); }
            void ReadIntByType(uint32_t &val) { val = ReadUint32(); }
            void ReadIntByType(uint64_t &val) { val = ReadUint64(); }

            void ReadIntByType(int8_t &val) { val = int8_t(ReadUint8()); }
            void ReadIntByType(int16_t &val) { val = int16_t(ReadUint16()); }
            void ReadIntByType(int32_t &val) { val = int32_t(ReadUint32()); }
            void ReadIntByType(int64_t &val) { val = int64_t(ReadUint64()); }

            template <typename Type>
            void Read(Type &val)
            {
                using Action = details::FromBinaryAction<typename details::ChooseStrategy<Type>::Strategy, Type>;

                Action::Impl(*this, val);
            }

        private:

            bool PrivateCanRead(size_t len)
            {
                return (m_DataLength - (m_ReadPtr - m_Data)) >= len;
            }

            const uint8_t *m_Data;
            size_t m_DataLength;
            const uint8_t *m_ReadPtr;
            bool m_HasError;
            std::string m_ErrorStr;
        };

    } // namespace bbox::enc
} // namespace bbox

#include <bbox/enc/details/FromBinaryAction.h>

#endif // __BBOX__ENC__FROM_ENCARY_H__
