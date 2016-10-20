/**
 * @file
 *
 * Header file for the bbox::enc::api::describe::details type-traits and helpers.
 */

#ifndef __BBOX__ENC__API__DESCRIBE__TYPE_TRAITS_H__
#define __BBOX__ENC__API__DESCRIBE__TYPE_TRAITS_H__

#include <type_traits>
#include <memory>
#include <tuple>
#include <bbox/enc/api/describe/Directions.h>
#include <bbox/enc/api/Param.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {
            // Forward declaration
            class Param;

            namespace describe
            {
                namespace details
                {

                    // Forward declarations

                    template <typename ValueType, typename DirectionType>
                    class ParamImpl;

                    template  <typename... ParamImplTypes>
                    class ParamPackImpl;

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // IsDirectionType
                    //

                    template <typename Type>
                    struct IsDirectionType : public std::false_type {};

                    template <>
                    struct IsDirectionType<bbox::enc::api::describe::Input> : std::true_type {};
                    template <>
                    struct IsDirectionType<bbox::enc::api::describe::Output> : std::true_type {};
                    template <>
                    struct IsDirectionType<bbox::enc::api::describe::InOut> : std::true_type {};

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // IsParamImplType
                    //

                    template <typename Type>
                    struct IsParamImplType : public std::false_type {};

                    template <typename _ValueType, typename _DirectionType>
                    struct IsParamImplType<ParamImpl<_ValueType, _DirectionType>> : public std::true_type
                    {
                        using ValueType = _ValueType;
                        using DirectionType = _DirectionType;

                        static_assert(IsDirectionType<DirectionType>::value, "Direction type must be on of bbox::api::describe::{Input, Output or InOut}");
                        static_assert(std::is_same<ValueType, typename std::decay<ValueType>::type>::value, "Value type must not include cv-qualifiers of references");
                        static_assert(!std::is_pointer<ValueType>::value, "Value type must not be a pointer type");
                        static_assert(!std::is_function<ValueType>::value, "Value type must not be a function type");
                    };

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // IsParamPackImplType
                    //

                    template <typename Type>
                    struct IsParamPackImplType  : public std::false_type {};

                    template <typename... ParamImplTypes>
                    struct IsParamPackImplType<ParamPackImpl<ParamImplTypes...>> : public std::true_type {};

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // GetTupleType
                    //

                    template <size_t Index, typename TupleType>
                    struct GetTupleType;

                    template <size_t Index, typename First, typename... Rest>
                    struct GetTupleType<Index, std::tuple<First, Rest...>>
                    {
                        using type = typename GetTupleType<Index - 1, std::tuple<Rest...>>::type;
                    };

                    template <typename First, typename... Rest>
                    struct GetTupleType<0, std::tuple<First, Rest...>>
                    {
                        using type = First;
                    };

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // Unpack
                    //

                    template <size_t Index, typename ParamPackImplType, bool MoreLeft = (Index < std::tuple_size<typename ParamPackImplType::ValuesTupleType>::value)>
                    struct Unpack
                    {
                        using ValuesTupleType = typename ParamPackImplType::ValuesTupleType;
                        using DirectionsTupleType = typename ParamPackImplType::DirectionsTupleType;

                        static void ToBinary(enc::ToBinary &m, const bool for_input, const ValuesTupleType &tuple)
                        {
                        }

                        static void ToTextFormat(enc::ToTextFormat &m, const bool for_input, const std::unique_ptr<api::Param> *param_ptrs, const ValuesTupleType &tuple)
                        {
                        }

                        static void FromBinary(enc::FromBinary &m, const bool for_input, ValuesTupleType &tuple)
                        {
                        }

                        static void FromTextFormat(enc::FromTextFormat &m, const bool for_input, const std::unique_ptr<api::Param> *param_ptrs, ValuesTupleType &tuple)
                        {
                        }
                    };

                    template <size_t Index, typename ParamPackImplType>
                    struct Unpack<Index, ParamPackImplType, true>
                    {
                        using ValuesTupleType = typename ParamPackImplType::ValuesTupleType;
                        using DirectionsTupleType = typename ParamPackImplType::DirectionsTupleType;
                        using DirectionType = typename GetTupleType<Index, DirectionsTupleType>::type;

                        static void ToBinary(enc::ToBinary &m, const bool for_input, const ValuesTupleType &tuple)
                        {
                            if (DirectionType::Include(for_input))
                            {
                                m.Write(std::get<Index>(tuple));
                            }

                            Unpack<Index + 1, ParamPackImplType>::ToBinary(m, for_input, tuple);
                        }

                        static void ToTextFormat(enc::ToTextFormat &m, const bool for_input, const std::unique_ptr<api::Param> *param_ptrs, const ValuesTupleType &tuple)
                        {
                            if (DirectionType::Include(for_input))
                            {
                                m.AddNamedValue(param_ptrs[Index]->GetName().c_str(), std::get<Index>(tuple));
                            }

                            Unpack<Index + 1, ParamPackImplType>::ToTextFormat(m, for_input, param_ptrs, tuple);
                        }

                        static void FromBinary(enc::FromBinary &m, const bool for_input, ValuesTupleType &tuple)
                        {
                            if (DirectionType::Include(for_input))
                            {
                                m.Read(std::get<Index>(tuple));
                            }

                            Unpack<Index + 1, ParamPackImplType>::FromBinary(m, for_input, tuple);
                        }

                        static void FromTextFormat(enc::FromTextFormat &m, const bool for_input, const std::unique_ptr<api::Param> *param_ptrs, ValuesTupleType &tuple)
                        {
                            if (DirectionType::Include(for_input))
                            {
                                m.DecodeNamedValue(param_ptrs[Index]->GetName().c_str(), std::get<Index>(tuple));
                            }

                            Unpack<Index + 1, ParamPackImplType>::FromTextFormat(m, for_input, param_ptrs, tuple);
                        }
                    };

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // CheckFunctionArgument
                    //

                    template <typename ArgumentType,
                            typename ParamImplType>
                    struct CheckFunctionArgument;

                    template <typename ArgumentType,
                              typename ValueType,
                              typename DirectionType>
                    struct CheckFunctionArgument<ArgumentType, ParamImpl<ValueType, DirectionType>>
                    {
                        static constexpr bool IsInput = DirectionType::IsInput;
                        static constexpr bool IsOutput = DirectionType::IsOutput;

                        static_assert(std::is_same<typename std::decay<ArgumentType>::type, ValueType>::value,
                            "Argument types must match exactly - even input parameters cannot be different (but assignable) types");

                        static_assert(!std::is_rvalue_reference<ArgumentType>::value,
                            "Argument cannot be by r-value reference");

                        static_assert(!std::is_volatile<typename std::remove_reference<ArgumentType>::type>::value,
                            "Argument cannot be volatile qualified");

                        static_assert(!IsOutput || !std::is_const<typename std::remove_reference<ArgumentType>::type>::value,
                            "InOut and Output arguments cannot be const qualified");

                        static_assert(!IsOutput || std::is_lvalue_reference<ArgumentType>::value,
                            "InOut and Output arguments must be by l-value reference");

                        static_assert(IsOutput
                                || !std::is_reference<ArgumentType>::value
                                || (std::is_reference<ArgumentType>::value && std::is_const<typename std::remove_reference<ArgumentType>::type>::value),
                            "Input arguments must be by value or by const reference");

                        static constexpr bool value = true;
                    };

                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // All
                    //

                    template <typename... ValueTypes>
                    struct All;

                    template <>
                    struct All<>
                    {
                        static constexpr bool value = true;
                    };


                    template <typename First, typename... Rest>
                    struct All<First, Rest...>
                    {
                        static constexpr bool value = First::value && All<Rest...>::value;
                    };


                    // ////////////////////////////////////////////////////////////////////////////////////
                    //
                    // Apply
                    //

                    template <typename FunctionType,
                              typename TupleType,
                              size_t... Sequence>
                    decltype(auto) Apply(const FunctionType &func, TupleType &tuple, std::index_sequence<Sequence...> /*sequence*/)
                    {
                        return func(std::get<Sequence>(tuple)...);
                    }

                } // namespace bbox::enc::api::describe::details
            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE__TYPE_TRAITS_H__
