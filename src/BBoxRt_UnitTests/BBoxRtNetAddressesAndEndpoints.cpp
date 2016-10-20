#include <unittest/UnitTest.h>
#include <bbox/rt/net/IpAddress.h>
#include <bbox/rt/net/UdpEndpoint.h>
#include <bbox/TextCoding.h>

using namespace bbox::rt::net;
using Assert = unittest::Assert;

namespace BBoxRt_UnitTests
{
    UT_TEST_CLASS(AddressesAndEndpointsTests)
    {
    public:
    
        AddressesAndEndpointsTests()
        {
            UT_REGISTER_METHOD(IpAddressTest);
            UT_REGISTER_METHOD(UdpEndpointTest)
        }

        UT_TEST_METHOD(IpAddressTest)
        {
            std::vector<std::pair<std::string, bool>> strings = {
                { "", false },
                { "asdf", false },

                { "0.0.0.0.0", false },
                { "0.0.0.0 ", false },
                { "256.0.0.0", false },
                { "0.0.0.256", false },
                { "0.0.0.0", true },
                { "127.0.0.1", true },
                { "255.255.255.255", true },

                { "::", true },
                { "::1", true },
                { "2001:db8:85a3::8a2e:370:7334", true },
                { "2001:0db8:85a3:0000:0000:8a2e:0370:7334", true },
                { "2001:db8::1", true },
                { "fc00::", true },

                { "fc00:: ", false },
                { "fc00::1::2", false },

            };

            for (const auto &element : strings)
            {
                const std::string &str_val = element.first;
                const bool expect_success = element.second;

                const std::string test_message = "IpAddress Test " + str_val;

                if (expect_success)
                {
                    IpAddress addr1;

                    Assert::IsTrue(IpAddress::FromString(str_val, addr1), test_message, UT_LINE_INFO());

                    std::string round_trip = addr1.ToString();
                    IpAddress addr2;

                    Assert::IsTrue(IpAddress::FromString(round_trip, addr2), test_message, UT_LINE_INFO());
                    Assert::AreEqual(addr1, addr2, test_message, UT_LINE_INFO());
                    Assert::AreEqual(addr1.ToString(), addr2.ToString(), test_message, UT_LINE_INFO());
                }
                else
                {
                    IpAddress addr1;

                    Assert::IsFalse(IpAddress::FromString(str_val, addr1), test_message, UT_LINE_INFO());
                }
            }
        }

        UT_TEST_METHOD(UdpEndpointTest)
        {
            std::vector<std::pair<std::string, bool>> strings = {
                { "0.0.0.0:0", true },
                { "", false },
                { "asdf", false },

                { "0.0.0.0.0", false },
                { "0.0.0.0 ", false },
                { "0.0.0.0:", false },
                { "0.0.0.0:a", false },
                { "0.0.0.0:0a", false },

                { "0.0.0.0:0", true },
                { "127.0.0.1:1234", true },
                { "255.255.255.255:65535", true },
                { "255.255.255.255:65536", false },
                { "255.255.255.255:111111111", false },

                { "[::]:0", true },
                { "[::1]:65535", true },
                { "[2001:db8:85a3::8a2e:370:7334]:65535", true },
                { "[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:0", true },
                { "[2001:db8::1]:0", true },
                { "[fc00::]:65535", true },
                { "[fc00::]:65536", false },
                { "[fc00::]:11111111111", false },

                { "[0.0.0.0]:0", false },
                { "2001:0db8:85a3:0000:0000:8a2e:0370:7334:0", false },

            };

            for (const auto &element : strings)
            {
                const std::string &str_val = element.first;
                const bool expect_success = element.second;

                const std::string test_message = "UdpEndpoint Test " + str_val;

                if (expect_success)
                {
                    UdpEndpoint ep1;

                    Assert::IsTrue(UdpEndpoint::FromString(str_val, ep1), test_message, UT_LINE_INFO());

                    std::string round_trip = ep1.ToString();
                    UdpEndpoint ep2;

                    Assert::IsTrue(UdpEndpoint::FromString(round_trip, ep2), test_message, UT_LINE_INFO());
                    Assert::AreEqual(ep1, ep2, test_message, UT_LINE_INFO());
                    Assert::AreEqual(ep1.ToString(), ep2.ToString(), test_message, UT_LINE_INFO());
                }
                else
                {
                    UdpEndpoint ep1;

                    Assert::IsFalse(UdpEndpoint::FromString(str_val, ep1), test_message, UT_LINE_INFO());
                }
            }
        }

    };
    
    UT_REGISTER_CLASS(AddressesAndEndpointsTests);
}
