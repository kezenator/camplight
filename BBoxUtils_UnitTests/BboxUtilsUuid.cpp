#include <unittest/UnitTest.h>

#include <bbox/Uuid.h>
#include <bbox/TextCoding.h>
#include <boost/algorithm/string/case_conv.hpp>

using namespace bbox;
using Assert = unittest::Assert;

namespace BBoxUtils_UnitTests
{
    UT_TEST_CLASS(UuidTests)
    {
        static void Verify(const Uuid &uuid, const std::string &expected, const char *name)
        {
            Assert::AreEqual(expected, uuid.ToString(), name, UT_LINE_INFO());

            // Test the round trip works

            Uuid from_string;
            Assert::IsTrue(Uuid::FromString(expected, from_string), name, UT_LINE_INFO());
            Assert::AreEqual(uuid, from_string, name, UT_LINE_INFO());
            Assert::AreEqual(expected, from_string.ToString(), name, UT_LINE_INFO());

            // Test that braceless output and round trip works

            std::string braceless_string = expected;
            braceless_string.pop_back();
            braceless_string.erase(0, 1);

            Uuid from_braceless;
            Assert::IsTrue(Uuid::FromString(braceless_string, from_braceless), name, UT_LINE_INFO());
            Assert::AreEqual(uuid, from_braceless, name, UT_LINE_INFO());
            Assert::AreEqual(braceless_string, from_braceless.ToBracelessString(), name, UT_LINE_INFO());

            // Test that the URN output and round trip works

            std::string urn_string = expected;
            urn_string.pop_back();
            urn_string.erase(0, 1);
            urn_string.insert(0, "urn:uuid:");

            Uuid from_urn;
            Assert::IsTrue(Uuid::FromString(urn_string, from_urn), name, UT_LINE_INFO());
            Assert::AreEqual(uuid, from_urn, name, UT_LINE_INFO());
            Assert::AreEqual(urn_string, from_urn.ToUrn(), name, UT_LINE_INFO());

            // Check that the short form is acceptable

            std::string short_string = expected;
            urn_string.erase(37, 1);
            urn_string.erase(24, 1);
            urn_string.erase(19, 1);
            urn_string.erase(14, 1);
            urn_string.erase(9, 1);
            urn_string.erase(0, 1);

            Uuid from_short;
            Assert::IsTrue(Uuid::FromString(short_string, from_short), name, UT_LINE_INFO());
            Assert::AreEqual(uuid, from_short, name, UT_LINE_INFO());
            Assert::AreEqual(expected, from_short.ToString(), name, UT_LINE_INFO());

            // Finally, check that lower case doesn't matter

            std::string lower_string = expected;
            boost::algorithm::to_lower(lower_string);

            Uuid from_lower;
            Assert::IsTrue(Uuid::FromString(lower_string, from_lower), name, UT_LINE_INFO());
            Assert::AreEqual(uuid, from_lower, name, UT_LINE_INFO());
            Assert::AreEqual(expected, from_lower.ToString(), name, UT_LINE_INFO());
        }

    public:
    
        UuidTests()
        {
            UT_REGISTER_METHOD(BasicUuidTest);
            UT_REGISTER_METHOD(InvalidUuidStringTest);
            UT_REGISTER_METHOD(UuidGenerateNameTest);
            UT_REGISTER_METHOD(UuidGenerateRandomTest);
        }

        UT_TEST_METHOD(BasicUuidTest)
        {
            Uuid null_uuid;

            Verify(null_uuid, "{00000000-0000-0000-0000-000000000000}", "null_uuid");

            Uuid sequential_uuid = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

            Verify(sequential_uuid, "{01020304-0506-0708-090A-0B0C0D0E0F10}", "sequential_uuid");

            Uuid copied_uuid1 = sequential_uuid;

            Verify(copied_uuid1, "{01020304-0506-0708-090A-0B0C0D0E0F10}", "copied_uuid1");

            Uuid copied_uuid2(sequential_uuid);

            Verify(copied_uuid2, "{01020304-0506-0708-090A-0B0C0D0E0F10}", "copied_uuid2");
        }

        UT_TEST_METHOD(InvalidUuidStringTest)
        {
            Uuid uuid;

            Assert::IsFalse(Uuid::FromString("", uuid),                                                 "empty", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{q1020304-0506-0708-090A-0B0C0D0E0F10}", uuid),           "wrong start", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A-0B0C0D0E0F1q}", uuid),           "wrong end", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A-0B0C0D0E0F1}", uuid),            "missing digit", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A-0B0C0D0E0F}", uuid),             "missing byte", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A-0B0C0D0E0F10", uuid),            "missing end brace", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("01020304-0506-0708-090A-0B0C0D0E0F10}", uuid),            "missing start brace", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("urn:asdf:01020304-0506-0708-090A-0B0C0D0E0F10", uuid),    "wrong urn", UT_LINE_INFO());

            Assert::IsFalse(Uuid::FromString("{0102030-40506-0708-090A-0B0C0D0E0F10}", uuid),           "wrong dash digit 1", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-050-60708-090A-0B0C0D0E0F10}", uuid),           "wrong dash digit 2", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-070-8090A-0B0C0D0E0F10}", uuid),           "wrong dash digit 3", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090-A0B0C0D0E0F10}", uuid),           "wrong dash digit 4", UT_LINE_INFO());

            Assert::IsFalse(Uuid::FromString("{0102030405-06-0708-090A-0B0C0D0E0F10}", uuid),           "wrong dash byte 1", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-050607-08-090A-0B0C0D0E0F10}", uuid),           "wrong dash byte 2", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-070809-0A-0B0C0D0E0F10}", uuid),           "wrong dash byte 3", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A0B-0C0D0E0F10}", uuid),           "wrong dash byte 4", UT_LINE_INFO());

            Assert::IsFalse(Uuid::FromString("{010203040506-0708-090A-0B0C0D0E0F10}", uuid),            "missing dash 1", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-05060708-090A-0B0C0D0E0F10}", uuid),            "missing dash 2", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708090A-0B0C0D0E0F10}", uuid),            "missing dash 3", UT_LINE_INFO());
            Assert::IsFalse(Uuid::FromString("{01020304-0506-0708-090A0B0C0D0E0F10}", uuid),            "missing dash 4", UT_LINE_INFO());
        }


        UT_TEST_METHOD(UuidGenerateNameTest)
        {
            // Generate a couple namespaces

            Uuid null_uuid;

            Verify(null_uuid, "{00000000-0000-0000-0000-000000000000}", "null_uuid");

            Uuid sequential_uuid = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

            Verify(sequential_uuid, "{01020304-0506-0708-090A-0B0C0D0E0F10}", "sequential_uuid");

            // Generate names from these namespaces

            Uuid null_name_asdf_1, null_name_asdf_2;
            Uuid null_name_zxcv_1, null_name_zxcv_2;
            Uuid sequential_name_asdf_1, sequential_name_asdf_2;
            Uuid sequential_name_zxcv_1, sequential_name_zxcv_2;

            null_name_asdf_1.GenerateNamed(null_uuid, "asdf");
            null_name_asdf_2.GenerateNamed(null_uuid, "asdf");
            null_name_zxcv_1.GenerateNamed(null_uuid, "zxcv");
            null_name_zxcv_2.GenerateNamed(null_uuid, "zxcv");

            sequential_name_asdf_1.GenerateNamed(sequential_uuid, "asdf");
            sequential_name_asdf_2.GenerateNamed(sequential_uuid, "asdf");
            sequential_name_zxcv_1.GenerateNamed(sequential_uuid, "zxcv");
            sequential_name_zxcv_2.GenerateNamed(sequential_uuid, "zxcv");

            // Check that the same name generated from the same namespace are equal

            Assert::AreEqual(null_name_asdf_1, null_name_asdf_2, "null_name_asdf_equa", UT_LINE_INFO());
            Assert::AreEqual(null_name_zxcv_1, null_name_zxcv_2, "null_name_zxcv_equa", UT_LINE_INFO());

            Assert::AreEqual(sequential_name_asdf_1, sequential_name_asdf_2, "sequential_name_asdf_equa", UT_LINE_INFO());
            Assert::AreEqual(sequential_name_zxcv_1, sequential_name_zxcv_2, "sequential_name_zxcv_equa", UT_LINE_INFO());

            // Check that different names generated from the same namespace are not equal

            Assert::AreNotEqual(null_name_asdf_1, null_name_zxcv_1, "null_1_not_equa", UT_LINE_INFO());
            Assert::AreNotEqual(null_name_asdf_2, null_name_zxcv_2, "null_2_not_equa", UT_LINE_INFO());

            Assert::AreNotEqual(sequential_name_asdf_1, sequential_name_zxcv_1, "sequential_1_not_equa", UT_LINE_INFO());
            Assert::AreNotEqual(sequential_name_asdf_2, sequential_name_zxcv_2, "sequential_2_not_equa", UT_LINE_INFO());

            // Check that the same names generated from different namespaces are not equal

            Assert::AreNotEqual(null_name_asdf_1, sequential_name_asdf_1, "diff_namespace_asdf_not_equa", UT_LINE_INFO());
            Assert::AreNotEqual(null_name_zxcv_1, sequential_name_zxcv_1, "diff_namespace_zxcv_not_equa", UT_LINE_INFO());

            // Check that the generated names have fixed values - i.e. they're not dependant on time

            Verify(null_name_asdf_1,        "{83B161EE-2B76-57B4-A3CE-80ADED239A85}", "null_asdf_value");
            Verify(null_name_zxcv_1,        "{B4A33F08-18B0-5986-9E87-4B937BDE1F06}", "null_zxcv_value");
            Verify(sequential_name_asdf_1,  "{12D79669-93AB-5EEF-BCB7-62544B432290}", "sequential_asdf_value");
            Verify(sequential_name_zxcv_1,  "{7266EE75-383B-5946-9B23-95A5157FD25D}", "sequential_zxcv_value");
        }
        
        UT_TEST_METHOD(UuidGenerateRandomTest)
        {
            Uuid random1, random2;

            random1.GenerateRandom();
            random2.GenerateRandom();

            Assert::AreNotEqual(random1, random2, "random_generate_not_equa", UT_LINE_INFO());
        }
    };
    UT_REGISTER_CLASS(UuidTests);
}
