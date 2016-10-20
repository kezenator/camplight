/**
 * @file
 *
 * Implementation file for the bbox::enc::api::MethodSet class.
 */

#include <bbox/enc/api/MethodSet.h>
#include <bbox/Assert.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {

            MethodSet::~MethodSet()
            {
            }

            std::set<std::string> MethodSet::GetMethodNames() const
            {
                std::set<std::string> result;

                for (const auto &entry : m_method_map)
                    result.insert(entry.first);

                return result;
            }

            const Method &MethodSet::GetMethodByName(const std::string &name) const
            {
                auto it = m_method_map.find(name);
                BBOX_ASSERT(it != m_method_map.end());

                return *it->second;
            }

            void MethodSet::Internal_AddMethod(std::unique_ptr<Method> &&method_ptr)
            {
                auto it = m_method_map.find(method_ptr->GetName());
                BBOX_ASSERT(it == m_method_map.end());

                m_method_map[method_ptr->GetName()] = std::move(method_ptr);
            }

            void MethodSet::CallEncodedMethod(FromTextFormat &input_text_coding, ToTextFormat &output_encoding)
            {
                boost::any tuple_value;
                std::string method_name;
                Method *method_ptr = nullptr;

                input_text_coding.StartStructure();
                input_text_coding.DecodeNamedValue("method", method_name);

                auto method_it = m_method_map.find(method_name);
                if (method_it == m_method_map.end())
                {
                    input_text_coding.SetError("Unknown method name");
                }
                else
                {
                    method_ptr = method_it->second.get();
                    tuple_value = method_ptr->GetParamPack().CreateDefaultConstructedTuple();

                    input_text_coding.StartNamedItem("params");
                    method_ptr->GetParamPack().TupleFromTextFormat(input_text_coding, true, tuple_value);
                    input_text_coding.CompleteNamedItem();
                    input_text_coding.CompleteStructure();
                    input_text_coding.EnsureAllDecoded();
                }

                bbox::Error error;

                if (input_text_coding.HasError())
                {
                    error = std::errc::bad_message;
                }
                else
                {
                    error = method_ptr->Call(tuple_value);
                }

                if (error)
                {
                    output_encoding.StartStructure();
                    output_encoding.StartNamedStructure("error");
                    output_encoding.AddNamedValue("description", error.ToString());
                    output_encoding.CompleteNamedStructure();
                    output_encoding.CompleteStructure();
                }
                else
                {
                    output_encoding.StartStructure();
                    output_encoding.StartNamedItem("results");
                    method_ptr->GetParamPack().TupleToTextFormat(output_encoding, false, tuple_value);
                    output_encoding.CompleteNamedItem();
                    output_encoding.CompleteStructure();
                }
            }

            void MethodSet::CallEncodedMethod(const std::string &method_name, FromTextFormat &input_text_coding, ToTextFormat &output_encoding)
            {
                boost::any tuple_value;
                Method *method_ptr = nullptr;

                auto method_it = m_method_map.find(method_name);
                if (method_it == m_method_map.end())
                {
                    input_text_coding.SetError("Unknown method name");
                }
                else
                {
                    method_ptr = method_it->second.get();
                    tuple_value = method_ptr->GetParamPack().CreateDefaultConstructedTuple();

                    method_ptr->GetParamPack().TupleFromTextFormat(input_text_coding, true, tuple_value);
                }

                bbox::Error error;

                if (input_text_coding.HasError())
                {
                    error = std::errc::bad_message;
                }
                else
                {
                    error = method_ptr->Call(tuple_value);
                }

                if (error)
                {
                    output_encoding.StartStructure();
                    output_encoding.StartNamedStructure("error");
                    output_encoding.AddNamedValue("description", error.ToString());
                    output_encoding.CompleteNamedStructure();
                    output_encoding.CompleteStructure();
                }
                else
                {
                    output_encoding.StartStructure();
                    output_encoding.StartNamedItem("results");
                    method_ptr->GetParamPack().TupleToTextFormat(output_encoding, false, tuple_value);
                    output_encoding.CompleteNamedItem();
                    output_encoding.CompleteStructure();
                }
            }

        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox
