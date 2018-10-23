#include <unittest/UnitTest.h>
#include <cstdint>

#include <bbox/Assert.h>
#include <bbox/enc/details/MethodDetection.h>
#include <bbox/enc/FromBinary.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToDebugString.h>
#include <bbox/enc/ToTextFormat.h>
#include <bbox/enc/ToJson.h>
#include <bbox/enc/ToXml.h>
#include <bbox/enc/FromJson.h>
#include <bbox/enc/FromXml.h>
#include <bbox/enc/MsgTypeLibrary.h>

#include <bbox/TypeInfo.h>
#include <bbox/Format.h>

#include <bbox/enc/api/Describe.h>
#include <bbox/enc/api/MethodSet.h>

namespace bbox
{
    namespace enc
    {
        namespace unittest
        {
            namespace enc_types
            {
                struct HasAll
                {
                    std::string name;
                    uint32_t age;

                    HasAll() = default;

                    HasAll(const std::string &_name, uint32_t _age)
                        : name(_name)
                        , age(_age)
                    {
                    }

                    void FromBinary(enc::FromBinary &m)
                    {
                        m.Read(name);
                        m.Read(age);
                    }

                    void ToBinary(enc::ToBinary &m) const
                    {
                        m.Write(name);
                        m.Write(age);
                    }

                    void ToTextFormat(enc::ToTextFormat &m) const
                    {
                        m.StartStructure();
                        m.AddNamedValue("name", name);
                        m.AddNamedValue("age", age);
                        m.CompleteStructure();
                    }

                    void FromTextFormat(enc::FromTextFormat &m)
                    {
                        m.StartStructure();
                        m.DecodeNamedValue("name", name);
                        m.DecodeNamedValue("age", age);
                        m.CompleteStructure();
                    }

                    bool operator ==(HasAll &other) const
                    {
                        return (name == other.name) && (age == other.age);
                    }

                    std::string ToString() const
                    {
                        // Shouldn't be called
                        BBOX_ASSERT(false);
                        return std::string();
                    }

                    static bool FromString(const std::string & /*str*/, HasAll & /*value*/)
                    {
                        // Shouldn't be called
                        BBOX_ASSERT(false);
                        return false;
                    }
                };

				bbox::enc::MsgTypeLibrary::Registration<HasAll> registration(
					"bbox::enc::unittest::enc_types::HasAll",
					[]()
					{
						return new_message<HasAll>();
					});

                struct FailDueToNonConstToMethods
                {
                    void FromBinary(enc::FromBinary &)
                    {
                    }

                    void ToBinary(enc::ToBinary &)
                    {
                    }

                    void ToTextFormat(enc::ToTextFormat &)
                    {
                    }

                    void FromTextFormat(enc::FromTextFormat &)
                    {
                    }

                    std::string ToString()
                    {
                        // Shouldn't be called
                        BBOX_ASSERT(false);
                        return std::string();
                    }

                    static bool FromString(const std::string & /*str*/, FailDueToNonConstToMethods & /*value*/)
                    {
                        // Shouldn't be called
                        BBOX_ASSERT(false);
                        return false;
                    }
                };
            } // namespace bbox::enc::unittest::enc_types

            template <typename Type>
            static void round_trip(const Type &value, const char *expected_json, const char *expected_debug_string)
            {
                // ::unittest::Log(bbox::Format("Testing type %s...", bbox::TypeInfo::Of<Type>().pretty_name()));

                // Convert the original value to a debug string, and see
                // that it matches the expected format

                std::string orig_to_debug_string = bbox::enc::ToDebugString(value);

                if (orig_to_debug_string != expected_debug_string)
                {
                    ::unittest::Log(bbox::Format("Failed orig_to_debug_string for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_debug_string));
                    ::unittest::Log(bbox::Format("Got:      %s", orig_to_debug_string));
                }

                BBOX_ASSERT(orig_to_debug_string == expected_debug_string);

                // Convert the original value to JSON, and see that
                // it matches the expected format

                std::string orig_to_json;

                {
                    bbox::enc::ToJson convert_to_json;
                    convert_to_json.SetValue(value);

                    orig_to_json = convert_to_json.ToString();
                }

                if (orig_to_json != expected_json)
                {
                    ::unittest::Log(bbox::Format("Failed orig_to_json for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_json));
                    ::unittest::Log(bbox::Format("Got:      %s", orig_to_json));
                }

                BBOX_ASSERT(orig_to_json == expected_json);

                // Now, convert it to binary

                std::vector<uint8_t> orig_to_binary;

                {
                    bbox::enc::ToBinary convert_to_binary;

                    convert_to_binary.Write(value);

                    BBOX_ASSERT(!convert_to_binary.HasError());

                    orig_to_binary = convert_to_binary.ExtractData();

                    BBOX_ASSERT(!orig_to_binary.empty());
                }
                
                // Convert back from binary

                Type rt_from_binary{};

                {
                    bbox::enc::FromBinary convert_from_binary(orig_to_binary.data(), orig_to_binary.size());

                    convert_from_binary.Read(rt_from_binary);

                    convert_from_binary.EnsureAllBytesRead();

                    if (convert_from_binary.HasError())
                    {
                        ::unittest::Log(bbox::Format("Failed convert_from_binary for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                        ::unittest::Log(bbox::Format("Error: %s", convert_from_binary.GetErrorString()));
                    }

                    BBOX_ASSERT(!convert_from_binary.HasError());
                }

                // Ensure that the round-trip to JSON also matches the expected value

                std::string rt_to_json;

                {
                    bbox::enc::ToJson convert_to_json;
                    convert_to_json.SetValue(rt_from_binary);

                    rt_to_json = convert_to_json.ToString();
                }

                if (rt_to_json != expected_json)
                {
                    ::unittest::Log(bbox::Format("Failed rt_to_json for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_json));
                    ::unittest::Log(bbox::Format("Got:      %s", rt_to_json));
                }

                BBOX_ASSERT(rt_to_json == expected_json);

                // Round trip the value back from JSON
                // and check that it matches

                Type rt_from_json{};

                {
                    bbox::enc::FromJson convert_from_json(expected_json);
                    convert_from_json.DecodeValue(rt_from_json);
                    convert_from_json.EnsureAllDecoded();

                    if (convert_from_json.HasError())
                    {
                        ::unittest::Log(bbox::Format("Failed convert_from_json for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                        ::unittest::Log(bbox::Format("Error: %s", convert_from_json.GetErrorString()));
                    }

                    BBOX_ASSERT(!convert_from_json.HasError());
                }

                // Check that the round trip back to json matches

                std::string rt_from_json_to_json;

                {
                    bbox::enc::ToJson convert_to_json;
                    convert_to_json.SetValue(rt_from_json);

                    rt_from_json_to_json = convert_to_json.ToString();
                }

                if (rt_from_json_to_json != expected_json)
                {
                    ::unittest::Log(bbox::Format("Failed rt_from_json_to_json for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_json));
                    ::unittest::Log(bbox::Format("Got:      %s", rt_from_json_to_json));
                }

                BBOX_ASSERT(rt_from_json_to_json == expected_json);

                // Now, convert to XML

                std::string orig_to_xml = bbox::enc::ToXml::ConvertToPretty("value", value);

                // Round-trip the value back from XML

                Type rt_from_xml{};

                {
                    bbox::enc::FromXml convert_from_xml(orig_to_xml);

                    convert_from_xml.DecodeNamedValue("value", rt_from_xml);

                    convert_from_xml.EnsureAllDecoded();

                    if (convert_from_xml.HasError())
                    {
                        ::unittest::Log(bbox::Format("Failed convert_from_xml for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                        ::unittest::Log(bbox::Format("Error: %s", convert_from_xml.GetErrorString()));
                    }

                    BBOX_ASSERT(!convert_from_xml.HasError());
                }

                // Ensure that the round-trip to JSON also matches the expected value

                std::string rt_from_xml_to_json;

                {
                    bbox::enc::ToJson convert_to_json;
                    convert_to_json.SetValue(rt_from_xml);

                    rt_from_xml_to_json = convert_to_json.ToString();
                }

                if (rt_from_xml_to_json != expected_json)
                {
                    ::unittest::Log(bbox::Format("Failed rt_from_xml_to_json for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_json));
                    ::unittest::Log(bbox::Format("Got:      %s", rt_from_xml_to_json));
                }

                BBOX_ASSERT(rt_from_xml_to_json == expected_json);
            }

            template <typename Type>
            static void round_trip_xml(const Type &value, const char *expected_xml_no_dec)
            {
                std::string expected_xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
                expected_xml += expected_xml_no_dec;

                // ::unittest::Log(bbox::Format("Testing round_trip_xml<%s>...", bbox::TypeInfo::Of<Type>().pretty_name()));

                // Convert the original value to XML, and see
                // that it matches the expected format

                std::string orig_to_xml;

                {
                    tinyxml2::XMLDocument doc;
                    bbox::enc::ToXml to_xml(doc, "round-trip", bbox::enc::ToTextFormat::MACHINE_ROUND_TRIP);
                    to_xml.SetValue(value);

                    tinyxml2::XMLPrinter printer(nullptr, true);
                    doc.Print(&printer);

                    orig_to_xml = printer.CStr();
                }

                if (orig_to_xml != expected_xml)
                {
                    ::unittest::Log(bbox::Format("Failed orig_to_xml for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                    ::unittest::Log(bbox::Format("Expected: %s", expected_xml));
                    ::unittest::Log(bbox::Format("Got:      %s", orig_to_xml));
                }

                BBOX_ASSERT(orig_to_xml == expected_xml);

				// Create a new value decoded from the XML

                Type rt_from_xml;

                {
                    bbox::enc::FromXml convert_from_xml(expected_xml);

                    convert_from_xml.StartNamedItem("round-trip");
                    convert_from_xml.DecodeValue(rt_from_xml);
                    convert_from_xml.CompleteNamedItem();

                    if (convert_from_xml.HasError())
                    {
                        ::unittest::Log(bbox::Format("Failed convert_from_xml for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
                        ::unittest::Log(bbox::Format("Error: %s", convert_from_xml.GetErrorString()));
                    }

                    BBOX_ASSERT(!convert_from_xml.HasError());
                }

				// Now encode this object to XML
				// and check that it also matches the expected value

				std::string rt_to_xml;

				{
					tinyxml2::XMLDocument doc;
					bbox::enc::ToXml to_xml(doc, "round-trip", bbox::enc::ToTextFormat::MACHINE_ROUND_TRIP);
					to_xml.SetValue(rt_from_xml);

					tinyxml2::XMLPrinter printer(nullptr, true);
					doc.Print(&printer);

					rt_to_xml = printer.CStr();
				}

				if (rt_to_xml != expected_xml)
				{
					::unittest::Log(bbox::Format("Failed rt_to_xml for type %s", bbox::TypeInfo::Of<Type>().pretty_name()));
					::unittest::Log(bbox::Format("Expected: %s", expected_xml));
					::unittest::Log(bbox::Format("Got:      %s", rt_to_xml));
				}

				BBOX_ASSERT(rt_to_xml == expected_xml);
			}

            UT_TEST_CLASS(TestEnc)
            {
            public:

                TestEnc()
                {
                    UT_REGISTER_METHOD(Test_bbox_enc_details_MethodDetection);
                    UT_REGISTER_METHOD(Test_bbox_enc_RoundTrip);
                    UT_REGISTER_METHOD(Test_bbox_enc_api_Describe);
                    UT_REGISTER_METHOD(Test_bbox_enc_api_MethodSet);
                }

                UT_TEST_METHOD(Test_bbox_enc_details_MethodDetection)
                {
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasFromBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasToBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasToTextFormat);
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasFromTextFormat);
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasToString);
                    BBOX_ASSERT(!enc::details::MethodDetection<uint32_t>::HasFromString);

                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasFromBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasToBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasToTextFormat);
                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasFromTextFormat);
                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasToString);
                    BBOX_ASSERT(!enc::details::MethodDetection<std::string>::HasFromString);

                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasFromBinary);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasToBinary);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasToTextFormat);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasFromTextFormat);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasToString);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::HasAll>::HasFromString);

                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasFromBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasToBinary);
                    BBOX_ASSERT(!enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasToTextFormat);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasFromTextFormat);
                    BBOX_ASSERT(!enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasToString);
                    BBOX_ASSERT(enc::details::MethodDetection<enc_types::FailDueToNonConstToMethods>::HasFromString);
                }

                UT_TEST_METHOD(Test_bbox_enc_RoundTrip)
                {
                    round_trip<std::string>("", "\"\"", "\"\"");
                    round_trip<std::string>("asdf", "\"asdf\"", "\"asdf\"");
                    round_trip<std::string>("string \n with \" escape \\ chars",
                                            "\"string \\n with \\\" escape \\\\ chars\"",
                                            "\"string \\n with \\\" escape \\\\ chars\"");

                    round_trip<bool>(false, "\"false\"", "\"false\"");
                    round_trip<bool>(true, "\"true\"", "\"true\"");

                    round_trip<uint8_t>(0, "\"0\"", "\"0\"");
                    round_trip<uint8_t>(255U, "\"255\"", "\"255\"");

                    round_trip<uint16_t>(0, "\"0\"", "\"0\"");
                    round_trip<uint16_t>(65535U, "\"65535\"", "\"65535\"");

                    round_trip<uint32_t>(0, "\"0\"", "\"0\"");
                    round_trip<uint32_t>(4294967295U, "\"4294967295\"", "\"4294967295\"");

                    round_trip<uint64_t>(0, "\"0\"", "\"0\"");
                    round_trip<uint64_t>(18446744073709551615ULL, "\"18446744073709551615\"", "\"18446744073709551615\"");

                    round_trip<int8_t>(-128, "\"-128\"", "\"-128\"");
                    round_trip<int8_t>(0, "\"0\"", "\"0\"");
                    round_trip<int8_t>(127, "\"127\"", "\"127\"");

                    round_trip<int16_t>(-32768, "\"-32768\"", "\"-32768\"");
                    round_trip<int16_t>(0, "\"0\"", "\"0\"");
                    round_trip<int16_t>(32767, "\"32767\"", "\"32767\"");

                    round_trip<int32_t>(-2147483648LL, "\"-2147483648\"", "\"-2147483648\"");
                    round_trip<int32_t>(0, "\"0\"", "\"0\"");
                    round_trip<int32_t>(2147483647LL, "\"2147483647\"", "\"2147483647\"");

                    round_trip<int64_t>(-9223372036854775807LL - 1, "\"-9223372036854775808\"", "\"-9223372036854775808\"");
                    round_trip<int64_t>(0, "\"0\"", "\"0\"");
                    round_trip<int64_t>(9223372036854775807LL, "\"9223372036854775807\"", "\"9223372036854775807\"");

                    round_trip<std::vector<uint8_t>>({},
                                                     "\"\"",
                                                     "\"Binary data, 0 bytes\"");
                    round_trip<std::vector<uint8_t>>({1, 2, 3},
                                                     "\"AQID\"",
                                                     "\"Binary data, 3 bytes, 010203\"");
                    round_trip<std::vector<uint8_t>>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22},
                                                     "\"AQIDBAUGBwgJCgsMDQ4PEBESExQVFg==\"",
                                                     "\"Binary data, 22 bytes, 0102030405060708090A0B0C0D0E0F1011121314...\"");

                    round_trip<enc_types::HasAll>(enc_types::HasAll("Mary", 32),
                                                  "{\"name\":\"Mary\",\"age\":\"32\"}",
                                                  "{name:\"Mary\", age:\"32\"}");

					round_trip<MsgPtr<enc_types::HasAll>>(new_message<enc_types::HasAll>("Baby", 1),
						"{\"type\":\"bbox::enc::unittest::enc_types::HasAll\",\"contents\":{\"name\":\"Baby\",\"age\":\"1\"}}",
						"{type:\"bbox::enc::unittest::enc_types::HasAll\", contents:{name:\"Baby\", age:\"1\"}}");
					round_trip<MsgAnyPtr>(new_message<enc_types::HasAll>("Toddler", 4),
						"{\"type\":\"bbox::enc::unittest::enc_types::HasAll\",\"contents\":{\"name\":\"Toddler\",\"age\":\"4\"}}",
						"{type:\"bbox::enc::unittest::enc_types::HasAll\", contents:{name:\"Toddler\", age:\"4\"}}");
					round_trip<MsgAnyPtr>(MsgAnyPtr(),
						"{\"type\":\"nullptr\"}",
						"{type:\"nullptr\"}");

					round_trip_xml<enc_types::HasAll>(enc_types::HasAll("simple", 32),
                        "<round-trip><name value=\"simple\"/><age value=\"32\"/></round-trip>");
                    round_trip_xml<enc_types::HasAll>(enc_types::HasAll("with\nnewlines", 32),
                        "<round-trip><name><![CDATA[with\nnewlines]]></name><age value=\"32\"/></round-trip>");

					round_trip_xml<MsgPtr<enc_types::HasAll>>(new_message<enc_types::HasAll>("Baby", 1),
						"<round-trip><type value=\"bbox::enc::unittest::enc_types::HasAll\"/><contents><name value=\"Baby\"/><age value=\"1\"/></contents></round-trip>");
					round_trip_xml<MsgAnyPtr>(new_message<enc_types::HasAll>("Toddler", 4),
						"<round-trip><type value=\"bbox::enc::unittest::enc_types::HasAll\"/><contents><name value=\"Toddler\"/><age value=\"4\"/></contents></round-trip>");
					round_trip_xml<MsgAnyPtr>(MsgAnyPtr(),
						"<round-trip><type value=\"nullptr\"/></round-trip>");
				}

                UT_TEST_METHOD(Test_bbox_enc_api_Describe)
                {
                    auto param1 =
                        bbox::enc::api::describe::Param<
                            std::string,
                            bbox::enc::api::describe::Input>(
                            "input",
                            "The name to copy to the output");

                    auto param2 =
                            bbox::enc::api::describe::Param<
                                enc_types::HasAll,
                                bbox::enc::api::describe::Output>(
                                "output",
                                "Output value - the name is copied from the input, the age is 32");

                    auto param_pack = bbox::enc::api::describe::ParamPack(std::move(param1), std::move(param2));

                    bbox::enc::ToBinary to_binary;
                    bbox::enc::FromBinary from_binary(nullptr, 0);
                    bbox::enc::ToJson to_json;
                    bbox::enc::FromXml from_xml("");

                    boost::any tuple = param_pack->CreateDefaultConstructedTuple();

                    BBOX_ASSERT(param_pack->TupleToDebugString(true, tuple) == "{input:\"\"}");
                    BBOX_ASSERT(param_pack->TupleToDebugString(false, tuple) == "{output:{name:\"\", age:\"0\"}}");

                    // Test that every function complies correctly and doesn't cause an exception

                    param_pack->TupleToBinary(to_binary, true, tuple);
                    param_pack->TupleToBinary(to_binary, false, tuple);

                    param_pack->TupleToTextFormat(to_json, true, tuple);
                    param_pack->TupleToTextFormat(to_json, false, tuple);

                    param_pack->TupleFromBinary(from_binary, true, tuple);
                    param_pack->TupleFromBinary(from_binary, false, tuple);

                    param_pack->TupleFromTextFormat(from_xml, true, tuple);
                    param_pack->TupleFromTextFormat(from_xml, false, tuple);

                    // Test that a filled-in param pack prints correctly.

                    std::tuple<std::string, enc_types::HasAll> filled_tuple{ "in_name", enc_types::HasAll("out_name", 56) };
                    boost::any filled_any{ filled_tuple };

                    BBOX_ASSERT(param_pack->TupleToDebugString(true, filled_any) == "{input:\"in_name\"}");
                    BBOX_ASSERT(param_pack->TupleToDebugString(false, filled_any) == "{output:{name:\"out_name\", age:\"56\"}}");

                    // Now, try and create a method

                    std::function<bbox::Error (const std::string &, enc_types::HasAll &)> func =
                        [](const std::string &input, enc_types::HasAll &output) -> bbox::Error
                        {
                            output.name = input;
                            output.age = 32;
                            return bbox::Error();
                        };

                    auto method = bbox::enc::api::describe::Method(
                        "method",
                        "Description of method",
                        std::move(param_pack),
                        std::move(func));

                    // See that calling the method does something

                    std::tuple<std::string, enc_types::HasAll> input_tuple{ "src_name", {"", 0} };
                    boost::any params_any{ input_tuple };

                    BBOX_ASSERT(method->GetParamPack().TupleToDebugString(true, params_any) == "{input:\"src_name\"}");
                    BBOX_ASSERT(method->GetParamPack().TupleToDebugString(false, params_any) == "{output:{name:\"\", age:\"0\"}}");

                    bbox::Error call_error = method->Call(params_any);

                    BBOX_ASSERT(!call_error);

                    BBOX_ASSERT(method->GetParamPack().TupleToDebugString(true, params_any) == "{input:\"src_name\"}");
                    BBOX_ASSERT(method->GetParamPack().TupleToDebugString(false, params_any) == "{output:{name:\"src_name\", age:\"32\"}}");
                }

                UT_TEST_METHOD(Test_bbox_enc_api_MethodSet)
                {
                    struct HasMethodAccess
                    {
                        bbox::enc::api::MethodSet m_method_set;
                        enc_types::HasAll m_value{ "", 0 };

                        bbox::Error GetValue(enc_types::HasAll &value)
                        {
                            value = m_value;
                            return bbox::Error();
                        }

                        bbox::Error SetValue(const enc_types::HasAll &value)
                        {
                            m_value = value;
                            return bbox::Error();
                        }

                        bbox::Error ReturnErrorIfNameNot(const std::string &name)
                        {
                            if (name != m_value.name)
                                return boost::system::errc::invalid_argument;
                            return bbox::Error();
                        }

                        HasMethodAccess()
                        {
                            m_method_set = bbox::enc::api::MethodSet(
                                bbox::enc::api::describe::Method(
                                    "GetValue",
                                    "Gets the current value",
                                    bbox::enc::api::describe::ParamPack(
                                        bbox::enc::api::describe::Param<
                                            enc_types::HasAll,
                                            bbox::enc::api::describe::Output>(
                                            "value",
                                            "The current value stored")),
                                    this,
                                    &HasMethodAccess::GetValue),
                                bbox::enc::api::describe::Method(
                                    "SetValue",
                                    "Sets the current value",
                                    bbox::enc::api::describe::ParamPack(
                                        bbox::enc::api::describe::Param<
                                            enc_types::HasAll,
                                            bbox::enc::api::describe::Input>(
                                            "value",
                                            "The new value to store")),
                                    this,
                                    &HasMethodAccess::SetValue),
                                bbox::enc::api::describe::Method(
                                    "ReturnErrorIfNameNot",
                                    "Returns an error if the name doesn't match the provided parameter, else returns no error",
                                    bbox::enc::api::describe::ParamPack(
                                        bbox::enc::api::describe::Param<
                                            std::string,
                                            bbox::enc::api::describe::Input>(
                                            "name",
                                            "The name to test against")),
                                    this,
                                    &HasMethodAccess::ReturnErrorIfNameNot));
                        }
                    };

                    HasMethodAccess method_access;

                    auto test_method_call = [&method_access](const char *input, const char *expected_output)
                    {
                        bbox::enc::FromJson from_json(input);
                        bbox::enc::ToJson to_json;

                        method_access.m_method_set.CallEncodedMethod(from_json, to_json);

                        std::string output = to_json.ToString();

                        if (output != expected_output)
                        {
                            ::unittest::Log(bbox::Format("Failed test_method_call"));
                            ::unittest::Log(bbox::Format("Input:    %s", input));
                            ::unittest::Log(bbox::Format("Output:   %s", output));
                            ::unittest::Log(bbox::Format("Expected: %s", expected_output));
                        }

                        BBOX_ASSERT(output == expected_output);
                    };

                    auto test_named_method_call = [&method_access](const char *method_name, const char *input, const char *expected_output)
                    {
                        bbox::enc::FromJson from_json(input);
                        bbox::enc::ToJson to_json;

                        method_access.m_method_set.CallEncodedMethod(method_name, from_json, to_json);

                        std::string output = to_json.ToString();

                        if (output != expected_output)
                        {
                            ::unittest::Log(bbox::Format("Failed test_named_method_call"));
                            ::unittest::Log(bbox::Format("Method:   %s", method_name));
                            ::unittest::Log(bbox::Format("Input:    %s", input));
                            ::unittest::Log(bbox::Format("Output:   %s", output));
                            ::unittest::Log(bbox::Format("Expected: %s", expected_output));
                        }

                        BBOX_ASSERT(output == expected_output);
                    };

                    // Test it's empty

                    test_method_call(
                        "{\"method\":\"GetValue\",\"params\":{}}",
                        "{\"results\":{\"value\":{\"name\":\"\",\"age\":\"0\"}}}");

                    // Test a success and failure test case

                    test_method_call(
                        "{\"method\":\"ReturnErrorIfNameNot\",\"params\":{\"name\":\"Joe\"}}",
                        "{\"error\":{\"description\":\"[\\\"Invalid argument\\\"/generic/22(0x00000016)]\"}}");

                    test_method_call(
                        "{\"method\":\"ReturnErrorIfNameNot\",\"params\":{\"name\":\"\"}}",
                        "{\"results\":{}}");

                    // Test the value hasn't changed

                    test_method_call(
                        "{\"method\":\"GetValue\",\"params\":{}}",
                        "{\"results\":{\"value\":{\"name\":\"\",\"age\":\"0\"}}}");
                    
                    // Change the value

                    test_method_call(
                        "{\"method\":\"SetValue\",\"params\":{\"value\":{\"name\":\"Joe\",\"age\":\"42\"}}}",
                        "{\"results\":{}}");

                    // Ensure it's changed

                    test_method_call(
                        "{\"method\":\"GetValue\",\"params\":{}}",
                        "{\"results\":{\"value\":{\"name\":\"Joe\",\"age\":\"42\"}}}");

                    // Confirm that the changed value changes which case causes an error

                    test_method_call(
                        "{\"method\":\"ReturnErrorIfNameNot\",\"params\":{\"name\":\"Joe\"}}",
                        "{\"results\":{}}");

                    test_method_call(
                        "{\"method\":\"ReturnErrorIfNameNot\",\"params\":{\"name\":\"\"}}",
                        "{\"error\":{\"description\":\"[\\\"Invalid argument\\\"/generic/22(0x00000016)]\"}}");

                    // Also a quick test than named methods work

                    test_named_method_call(
                        "GetValue",
                        "{}",
                        "{\"results\":{\"value\":{\"name\":\"Joe\",\"age\":\"42\"}}}");

                    test_named_method_call(
                        "SetValue",
                        "{\"value\":{\"name\":\"Joe\",\"age\":\"84\"}}",
                        "{\"results\":{}}");

                    test_named_method_call(
                        "GetValue",
                        "{}",
                        "{\"results\":{\"value\":{\"name\":\"Joe\",\"age\":\"84\"}}}");
                }

            }; // UT_TEST_CLASS(TestEnc)

            UT_REGISTER_CLASS(TestEnc);

        } // namespace bbox::enc::unittest
    } // namespace bbox::enc
} // namespace bbox
