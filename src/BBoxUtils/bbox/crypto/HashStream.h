/**
 * @file
 *
 * Header file for the bbox::crypto::HashStream class.
 */

#ifndef __BBOX__CRYPTO__HASH_STREAM_H__
#define __BBOX__CRYPTO__HASH_STREAM_H__

#pragma once

#include <bbox/crypto/Hash.h>

namespace bbox {
    namespace crypto {

        /**
         * A stream to produce a hash from
         * a sequence of bytes.
         */
        class HashStream
        {
            // Not allowed
            HashStream() = delete;
            HashStream(const HashStream &) = delete;
            void operator =(const HashStream &) = delete;

        public:

            /**
             * The type of hash.
             */
            enum E_TYPE
            {
                MD5,
                SHA_256,
                SHA_512,
                SHA1,
            };

            static size_t GetOutputLength(E_TYPE type);

            explicit HashStream(E_TYPE type);
            ~HashStream();

            void AddBytes(const void *data, size_t length);
            Hash CompleteHash();

        private:
            class State;

            State *m_state;
        };

    } // namespace bbox::crypto
} // namespace bbox

#endif // __BBOX__CRYPTO__HASH_STREAM_H__
