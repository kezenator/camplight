/**
 * @file
 *
 * Header file for the bbox::enc::api::describe functions.
 */

#ifndef __BBOX__ENC__API__DESCRIBE_H__
#define __BBOX__ENC__API__DESCRIBE_H__

#include <bbox/enc/api/describe/Directions.h>
#include <bbox/enc/api/describe/details/TypeTraits.h>
#include <bbox/enc/api/describe/details/ParamImpl.h>
#include <bbox/enc/api/describe/details/ParamPackImpl.h>
#include <bbox/enc/api/describe/details/MethodImpl.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {
            namespace describe
            {

                /**
                 * Describes a single input, output or in/out (reference) parameter to a method.
                 *
                 * The type parameter must be a value type that can be encoded using the bbox::enc library.
                 * The direction parameter must be one of bbox::enc::api::describe::Input, Output or InOut.
                 *
                 * The name parameter is the name of the parameter.
                 * The description parameter describes the parameter for documentation purposes.
                 *
                 * For example:
                 * @code
                 *     bbox::enc::api::desribe::Param<std::string, bbox::enc::api::describe::Input>("foo", "An input parameter called foo...");
                 * @endcode
                 */
                template <typename Type, typename Direction>
                auto Param(const char *name, const char *description)
                    -> typename details::ParamImpl<Type, Direction>::Ptr
                {
                    return std::make_unique<details::ParamImpl<Type, Direction>>(name, description);
                }

                /**
                 * Describes a parameter pack - i.e. zero, one or more parameters to be passed to or returned
                 * from a method.
                 *
                 * This function should be called with the result of zero or more calls
                 * to the bbox::enc::api::describe::Param function.
                 *
                 * For example:
                 * @code
                 *    bbox::enc::api::describe::ParamPack(
                 *        bbox::enc::api::describe::Param<std::string, bbox::enc::api::describe::Input>("input", "The string to convert to a uint64_t"),
                 *        bbox::enc::api::describe::Param<uint64_t, bbox::enc::api::describe::Output>("result", "The uint64_t value decoded from the string"));
                 * @endcode
                 */
                template <typename... ParamImplTypes>
                auto ParamPack(std::unique_ptr<ParamImplTypes> &&... params)
                    -> typename details::ParamPackImpl<ParamImplTypes...>::Ptr
                {
                    return std::make_unique<details::ParamPackImpl<ParamImplTypes...>>(std::move(params)...);
                }

                /**
                 * Describes a method.
                 */
                template <typename ParamPackImplType,
                          typename ReturnType,
                          typename... Arguments>
                decltype(auto) Method(
                    const char *name,
                    const char *description,
                    std::unique_ptr<ParamPackImplType> &&param_pack,
                    std::function<ReturnType (Arguments...)> &&func)
                {
                    return std::make_unique<details::MethodImpl<ParamPackImplType, ReturnType, Arguments...>>(
                        name, description, std::move(param_pack), std::move(func));
                }

                /**
                 * Describes a method bound to a C++ object.
                 */
                template <typename ParamPackImplType,
                          typename ObjectType,
                          typename ReturnType,
                          typename... Arguments>
                decltype(auto) Method(
                    const char *name,
                    const char *description,
                    std::unique_ptr<ParamPackImplType> &&param_pack,
                    ObjectType *object,
                    ReturnType (ObjectType::*method)(Arguments...))
                {
                    return Method(
                        name,
                        description,
                        std::move(param_pack),
                        std::function<ReturnType (Arguments...)>(
                            [=](Arguments... args) -> ReturnType
                            {
                                return (object->*method)(std::forward<Arguments>(args)...);
                            }));
                }

            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE_H__
