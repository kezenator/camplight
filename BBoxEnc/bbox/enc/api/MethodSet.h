/**
* @file
*
* Header file for the bbox::enc::api::MethodSet class.
*/

#ifndef __BBOX__ENC__API__METHOD_SET_H__
#define __BBOX__ENC__API__METHOD_SET_H__

#include <bbox/enc/api/Method.h>
#include <memory>

namespace bbox
{
    namespace enc
    {
        namespace api
        {

            /**
             * A collection of named methods.
             */
            class MethodSet
            {
                MethodSet(const MethodSet &) = delete;
                MethodSet &operator =(const MethodSet &) = delete;

                void Internal_AddMethods()
                {
                }

                template <typename First, typename... Rest>
                void Internal_AddMethods(First method_ptr, Rest... rest)
                {
                    Internal_AddMethod(std::unique_ptr<Method>(std::forward<First>(method_ptr)));
                    Internal_AddMethods(std::forward<Rest>(rest)...);
                }

                void Internal_AddMethod(std::unique_ptr<Method> &&method_ptr);

            public:
                MethodSet()
                {
                }

                template <typename... MethodImplTypes>
                MethodSet(typename std::unique_ptr<MethodImplTypes> &&... method_ptrs)
                {
                    Internal_AddMethods(std::forward<std::unique_ptr<MethodImplTypes>>(method_ptrs)...);
                }

                MethodSet(MethodSet &&other) = default;
                MethodSet &operator =(MethodSet &&other) = default;

                ~MethodSet();

                bool HasMethodWithName(const std::string &name) const
                {
                    return m_method_map.count(name) == 1;
                }

                std::set<std::string> GetMethodNames() const;
                const Method &GetMethodByName(const std::string &name) const;

                /**
                 * Decodes and executes a method call.
                 *
                 * For example:
                 * @code
                 *    std::string ExecuteJsonMethodCall(const std::string &input)
                 *    {
                 *        bbox::enc::FromJson from_json(input);
                 *        bbox::enc::ToJson to_json;
                 *
                 *        method_set.CallEncodedMethod(from_json, to_json);
                 *
                 *        return to_json.ToString();
                 *    }
                 *
                 *    ...
                 *
                 *    BBOX_ASSERT(
                 *        ExecuteJsonMethodCall("{\"method\":\"get_value\",\"params\":{}")
                 *        == "{\"results\":{\"output\":\"42\"}}");
                 * @endcode
                 */
                void CallEncodedMethod(FromTextFormat &input_text_coding, ToTextFormat &output_encoding);

                /**
                 * Decodes and executes a method call.
                 *
                 * For example:
                 * @code
                 *    std::string ExecuteJsonMethodCall(const std::string &method_name, const std::string &input)
                 *    {
                 *        bbox::enc::FromJson from_json(input);
                 *        bbox::enc::ToJson to_json;
                 *
                 *        method_set.CallEncodedMethod(method_name, from_json, to_json);
                 *
                 *        return to_json.ToString();
                 *    }
                 *
                 *    ...
                 *
                 *    BBOX_ASSERT(
                 *        ExecuteJsonMethodCall("get_value", "{}")
                 *        == "{\"results\":{\"output\":\"42\"}}");
                 * @endcode
                 */
                void CallEncodedMethod(const std::string &method_name, FromTextFormat &input_text_coding, ToTextFormat &output_encoding);

            private:

                std::map<std::string, std::unique_ptr<Method>> m_method_map;
            };

        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__METHOD_SET_H__
