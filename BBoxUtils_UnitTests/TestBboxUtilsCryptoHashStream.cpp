#include <unittest/UnitTest.h>

#include <bbox/Format.h>
#include <bbox/Base64.h>
#include <bbox/crypto/HashStream.h>

using namespace bbox;
using Assert = unittest::Assert;

namespace BBoxUtils_UnitTests
{
    UT_TEST_CLASS(CryptoHashStreamUnitTests)
    {

        static void VerifyDecoded(
            const std::string &src_str,
            const std::vector<uint8_t> &src_data,
            bbox::crypto::HashStream::E_TYPE hash_type,
            const std::string &hash_name,
            const std::string &expected)
        {
            // First, test as one big block of data

            {
                bbox::crypto::HashStream hash_stream(hash_type);

                if (src_data.size() != 0)
                {
                    hash_stream.AddBytes(src_data.data(), src_data.size());
                }

                Assert::AreEqual(
                    expected,
                    hash_stream.CompleteHash().ToBase64String(),
                    bbox::Format("%s %s Bulk", src_str, hash_name),
                    UT_LINE_INFO());

                //unittest::Log(bbox::Format("%s %s => %s", src_str, hash_name, hash_stream.CompleteHash().ToBase64String()));
            }

            // Also, test inserting one byte at a time

            {
                bbox::crypto::HashStream hash_stream(hash_type);

                for (size_t i = 0; i < src_data.size(); ++i)
                    hash_stream.AddBytes(&src_data[i], 1);

                Assert::AreEqual(
                    expected,
                    hash_stream.CompleteHash().ToBase64String(),
                    bbox::Format("%s %s Per-byte", src_str, hash_name),
                    UT_LINE_INFO());

                //unittest::Log(bbox::Format("%s %s => %s", src_str, hash_name, hash_stream.CompleteHash().ToBase64String()));
            }
        }

        static void Verify(
            const std::string &src,
            const std::string &expected_md5,
            const std::string &expected_sha1,
            const std::string &expected_sha256,
            const std::string &expected_sha512)
        {
            std::vector<uint8_t> src_data(src.size());

            for (size_t i = 0; i < src.size(); ++i)
                src_data[i] = src[i];
            
            VerifyDecoded(src, src_data, bbox::crypto::HashStream::MD5, "MD5", expected_md5);
            VerifyDecoded(src, src_data, bbox::crypto::HashStream::SHA1, "SHA1", expected_sha1);
            VerifyDecoded(src, src_data, bbox::crypto::HashStream::SHA_256, "SHA_256", expected_sha256);
            VerifyDecoded(src, src_data, bbox::crypto::HashStream::SHA_512, "SHA_512", expected_sha512);
        }

    public:
    
        CryptoHashStreamUnitTests()
        {
            UT_REGISTER_METHOD(Test_bbox_crypto_HashStream);
        }

        UT_TEST_METHOD(Test_bbox_crypto_HashStream)
        {
            Verify(
                "",
                "1B2M2Y8AsgTpgAmY7PhCfg==",
                "2jmj7l5rSw0yVb/vlWAYkK/YBwk=",
                "47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=",
                "z4PhNX7vuL3xVChQ1m2AB9Yg5AULVxXcg/SpIdNs6c5H0NE8XYXysP+DGNKHfuwvY7kxvUdBeoGlODJ6+SfaPg==");

            Verify(
                "a short string",
                "/Yd3X8O/qt+TsWY+35wJWw==",
                "me/lqk9q41qAb8Ed8esRb/gLKdM=",
                "eRnCOmq1y57si7kqv59xpUVLyElYSIZcnmZnxOCuOPI=",
                "NFxpiM6MAlzlrt1+G427tuDut3RUJskGkRUdKFYZaQhY1+wiqx96aPgZXWkRF85S7CTT2skl386fEw9DUQzfXQ==");

            Verify(
                "A long string that is longer than the length of any of the hashes, "
                    "and includes special characters and puctuation like 1234567890!@#$%^&*() as well as "
                    "some other characters like quotes \' \" and new lines \t \b \r \n "
                    "and finally a bit more text including bytes with the high bit set \x84 \xff \xde",
                "GyTFowdCDCkU+dYMOf3AeQ==",
                "jL3DvP3PmJ6roX3Oo2xk34nUA70=",
                "lA0ii2e+SjULVOUX4n2f6ij0LqiBgsaHS1Qjemru2GM=",
                "ig6jCijuPXVps4+pDhWHYw0aBPTYnL9SmEUsLIJFkKkxdtDua2a6RuDm3lnxCfzDHOYtIYk+KmriqxI1gafCPw==");
        }
    };
    UT_REGISTER_CLASS(CryptoHashStreamUnitTests);
}
