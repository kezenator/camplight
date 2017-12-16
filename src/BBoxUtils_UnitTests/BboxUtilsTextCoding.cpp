#include <unittest/UnitTest.h>

#include <bbox/TextCoding.h>
#include <bbox/Assert.h>

using namespace bbox;
using Assert = unittest::Assert;

namespace BBoxUtils_UnitTests
{
    UT_TEST_CLASS(Test_bbox_TextCoding)
    {
    public:
        Test_bbox_TextCoding()
        {
            UT_REGISTER_METHOD(Test_bbox_TextCoding_Newlines_DOS_to_UNIX);
            UT_REGISTER_METHOD(Test_bbox_TextCoding_Newlines_UNIX_to_DOS);
        }

        UT_TEST_METHOD(Test_bbox_TextCoding_Newlines_DOS_to_UNIX)
        {
            Assert::AreEqual(
                "one\ntwo\nthree\nfour\n\nsix",
                bbox::TextCoding::Newlines_DOS_to_UNIX("one\ntwo\r\nthree\r\nfour\n\r\nsix"),
                "dos to unix",
                UT_LINE_INFO());
        }

        UT_TEST_METHOD(Test_bbox_TextCoding_Newlines_UNIX_to_DOS)
        {
            Assert::AreEqual(
                "one\r\ntwo\r\nthree\r\nfour\r\n\r\nsix",
                bbox::TextCoding::Newlines_UNIX_to_DOS("one\ntwo\r\nthree\r\nfour\n\nsix"),
                "unix to dos",
                UT_LINE_INFO());
        }
    };
    UT_REGISTER_CLASS(Test_bbox_TextCoding);
}
