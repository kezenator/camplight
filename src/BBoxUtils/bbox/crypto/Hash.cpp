/**
 * @file
 *
 * Implementation file for the bbox::crypto::Hash class.
 */

#include <bbox/crypto/Hash.h>

#include <bbox/Base64.h>
#include <bbox/Base16.h>

namespace bbox {
    namespace crypto {

        std::string Hash::ToString() const
        {
            return Base16::Encode(m_value);
        }

        std::string Hash::ToBase64String() const
        {
            return Base64::Encode(m_value);
        }

    } // namespace bbox::crypto
} // namespace bbox
