/**
 * @file
 *
 * Implementation file for the bbox::crypto::HashStream class.
 */

#include <bbox/crypto/HashStream.h>
#include <bbox/Assert.h>

#ifdef WIN32

#include <Windows.h>
#include <Wincrypt.h>

#pragma comment(lib, "Advapi32.lib")

namespace bbox {
    namespace crypto {

        class HashStream::State
        {
        public:
            inline State(E_TYPE type)
            {
                BOOL aquire_result = ::CryptAcquireContext(
                    &m_provider,
                    NULL,
                    MS_ENH_RSA_AES_PROV,
                    PROV_RSA_AES,
                    CRYPT_VERIFYCONTEXT);

                BBOX_ASSERT(aquire_result);

                ALG_ID alg_id;

                switch (type)
                {
                case MD5:       alg_id = CALG_MD5; break;
                case SHA_256:   alg_id = CALG_SHA_256; break;
                case SHA_512:   alg_id = CALG_SHA_512; break;
                case SHA1:      alg_id = CALG_SHA1; break;
                default:        BBOX_ASSERT(false);
                }

                BOOL create_result = ::CryptCreateHash(
                    m_provider,
                    alg_id,
                    0,
                    0,
                    &m_hash);

                BBOX_ASSERT(create_result);
            }

            inline ~State()
            {
                ::CryptDestroyHash(m_hash);
                ::CryptReleaseContext(m_provider, 0);
            }

            HCRYPTPROV m_provider;
            HCRYPTHASH m_hash;
        };

        size_t HashStream::GetOutputLength(E_TYPE type)
        {
            switch (type)
            {
            case MD5:     return 16;
            case SHA_256: return 32;
            case SHA_512: return 64;
            case SHA1:    return 20;
            }
            
            // Unknown type
            BBOX_ASSERT(false);
            return 0;
        }

        HashStream::HashStream(E_TYPE type)
            : m_state(new State(type))
        {
        }

        HashStream::~HashStream()
        {
            delete m_state;
        }

        void HashStream::AddBytes(const void *data, size_t length)
        {
            BBOX_ASSERT(m_state);

            DWORD dword_length = DWORD(length);
            BBOX_ASSERT(dword_length == length);

            BOOL hash_result = ::CryptHashData(m_state->m_hash, static_cast<const BYTE *>(data), dword_length, 0);
            BBOX_ASSERT(hash_result);
        }

        Hash HashStream::CompleteHash()
        {
            BBOX_ASSERT(m_state);

            DWORD hash_len;

            DWORD get_len = sizeof(DWORD);
            BOOL get_result = ::CryptGetHashParam(
                m_state->m_hash,
                HP_HASHSIZE,
                reinterpret_cast<BYTE *>(&hash_len),
                &get_len,
                0);
            BBOX_ASSERT(get_result);
            BBOX_ASSERT(get_len == sizeof(DWORD));

            std::vector<uint8_t> hash_data;
            hash_data.resize(hash_len);

            get_len = hash_len;
            get_result = ::CryptGetHashParam(
                m_state->m_hash,
                HP_HASHVAL,
                &hash_data[0],
                &get_len,
                0);
            BBOX_ASSERT(get_result);
            BBOX_ASSERT(get_len == hash_len);

            delete m_state;
            m_state = nullptr;

            return Hash(std::move(hash_data));
        }

    } // namespace bbox::crypto
} // namespace bbox

#else // not WIN32

#include <openssl/evp.h>

namespace bbox {
    namespace crypto {

        class HashStream::State
        {
        public:
            inline State(E_TYPE type)
            {
                EVP_MD_CTX_init(&m_context);

                const EVP_MD *evp_type = nullptr;

                switch (type)
                {
                case MD5:       evp_type = EVP_md5(); break;
                case SHA_256:   evp_type = EVP_sha256(); break;
                case SHA_512:   evp_type = EVP_sha512(); break;
                case SHA1:      evp_type = EVP_sha1(); break;
                default:        BBOX_ASSERT(false);
                }

                int init_result = EVP_DigestInit(&m_context, evp_type);
                BBOX_ASSERT(init_result == 1);
            }

            inline ~State()
            {
                EVP_MD_CTX_cleanup(&m_context);
            }

            EVP_MD_CTX m_context;
        };

        HashStream::HashStream(E_TYPE type)
            : m_state(new State(type))
        {
        }

        HashStream::~HashStream()
        {
            delete m_state;
        }

        size_t HashStream::GetOutputLength(E_TYPE type)
        {
            switch (type)
            {
            case MD5:     return 16;
            case SHA_256: return 32;
            case SHA_512: return 64;
            case SHA1:    return 20;
            }
            
            // Unknown type
            BBOX_ASSERT(false);
            return 0;
        }

        void HashStream::AddBytes(const void *data, size_t length)
        {
            BBOX_ASSERT(m_state);

            int update_result = EVP_DigestUpdate(&m_state->m_context, data, length);
            BBOX_ASSERT(update_result == 1);
        }

        Hash HashStream::CompleteHash()
        {
            BBOX_ASSERT(m_state);

            unsigned char out_buffer[EVP_MAX_MD_SIZE];
            unsigned int out_size;

            int final_result = EVP_DigestFinal(
                                    &m_state->m_context,
                                    out_buffer,
                                    &out_size);

            BBOX_ASSERT(final_result == 1);
            BBOX_ASSERT(out_size <= EVP_MAX_MD_SIZE);

            delete m_state;
            m_state = nullptr;

            return Hash(std::vector<uint8_t>(out_buffer,
                                             out_buffer + out_size));
        }

    } // namespace bbox::crypto
} // namespace bbox

#endif // not WIN32