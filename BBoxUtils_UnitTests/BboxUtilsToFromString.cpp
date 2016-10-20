#include <unittest/UnitTest.h>

#include <bbox/ToString.h>
#include <bbox/FromString.h>
#include <bbox/Assert.h>

using namespace bbox;
using Assert = unittest::Assert;

namespace
{
    struct CustomType
    {
        std::string val;

        CustomType()
        {
        }

        explicit CustomType(std::string &&_val)
            : val(std::move(_val))
        {
        }

        const std::string &ToString() const
        {
            return val;
        }
        
        static bool FromString(const std::string &str, CustomType &val)
        {
            val.val = str;
            return true;
        }
    };
}

namespace BBoxUtils_UnitTests
{
    UT_TEST_CLASS(Test_bbox_ToString)
    {
    private:
        template <typename Type>
        static void Verify(Type &&val, const char *expected)
        {
            std::string str = bbox::ToString(std::move(val));

            Assert::AreEqual(expected, str, "Verify r-value", UT_LINE_INFO());

            typename std::remove_reference<Type>::type round_trip;
            Assert::IsTrue(bbox::FromString(str, round_trip), "FromString r-value", UT_LINE_INFO());

            Assert::AreEqual(expected, bbox::ToString(round_trip), "Round-trip r-value", UT_LINE_INFO());
        }

        template <typename Type>
        static void Verify(const Type &val, const char *expected)
        {
            std::string str = bbox::ToString(val);

            Assert::AreEqual(expected, str, "Verify l-value", UT_LINE_INFO());

            typename std::remove_reference<Type>::type round_trip;
            Assert::IsTrue(bbox::FromString(str, round_trip), "FromString l-value", UT_LINE_INFO());

            Assert::AreEqual(expected, bbox::ToString(round_trip), "Round-trip l-value", UT_LINE_INFO());
        }

    public:
    
        Test_bbox_ToString()
        {
            UT_REGISTER_METHOD(Test_bbox_ToString_All);
        }

        UT_TEST_METHOD(Test_bbox_ToString_All)
        {
            // Literals should work - but only for ToString

            Assert::AreEqual("asdf", bbox::ToString("asdf"), "initial asdf", UT_LINE_INFO());
            Assert::AreEqual("1", bbox::ToString(1), "initial 1", UT_LINE_INFO());
            Assert::AreEqual("0", bbox::ToString(0), "initial 0", UT_LINE_INFO());
            Assert::AreEqual("-1", bbox::ToString(-1), "initial -1", UT_LINE_INFO());

            // Strings

            Verify(std::string("1234"), "1234");

            {
                std::string val("zxcv");
                Verify(val, "zxcv");
            }

            // Integers

            {
                size_t val(123);
                Verify(val, "123");
            }

            {
                uint8_t val(123);
                Verify(val, "123");
            }

            {
                uint16_t val(12345);
                Verify(val, "12345");
            }

            {
                uint32_t val(123456);
                Verify(val, "123456");
            }

            {
                uint64_t val(1234567);
                Verify(val, "1234567");
            }

            {
                int8_t val(-123);
                Verify(val, "-123");
            }

            {
                int16_t val(-12345);
                Verify(val, "-12345");
            }

            {
                int32_t val(-123456);
                Verify(val, "-123456");
            }

            {
                int64_t val(-1234567);
                Verify(val, "-1234567");
            }

            // Custom type with ToString method

            Verify(CustomType("zxcv"), "zxcv");

            {
                CustomType custom_val("qwer");
                Verify(custom_val, "qwer");
            }
        }
    };
    UT_REGISTER_CLASS(Test_bbox_ToString);
}
