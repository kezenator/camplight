/**
 * @file
 *
 * Header file for the bbox::enc::api::describe::details::ParamPackImpl class.
 */

#ifndef __BBOX__ENC__API__DESCRIBE__PARAM_PACK_IMPL_H__
#define __BBOX__ENC__API__DESCRIBE__PARAM_PACK_IMPL_H__

#include <memory>
#include <array>
#include <tuple>

#include <bbox/Assert.h>
#include <bbox/enc/api/describe/details/TypeTraits.h>
#include <bbox/enc/api/ParamPack.h>
#include <bbox/enc/ToJson.h>

namespace bbox
{
    namespace enc
    {
        namespace api
        {
            namespace describe
            {
                namespace details
                {

                    template <typename... ParamImplTypes>
                    class ParamPackImpl : public ParamPack
                    {
                    private:

                        inline void SavePtrs(size_t index)
                        {
                        }

                        template <typename ParamImpl, typename... Rest>
                        inline void SavePtrs(size_t index, std::unique_ptr<ParamImpl> &&ptr, Rest &&... rest)
                        {
                            static_assert(IsParamImplType<ParamImpl>::value, "Arguments to bbox::enc::api::describe::ParamPack must only be the result of a call to bbox::enc::api::describe::Param");

                            m_param_ptrs[index] = std::move(ptr);
                            SavePtrs(index + 1, std::move(rest)...);
                        }

                    public:

                        using ValuesTupleType = std::tuple<decltype(typename details::IsParamImplType<ParamImplTypes>::ValueType())...>;
                        using DirectionsTupleType = std::tuple<decltype(typename details::IsParamImplType<ParamImplTypes>::DirectionType())...>;

                        static constexpr size_t NUM_PARAMS = sizeof...(ParamImplTypes);

                        using Ptr = std::unique_ptr<ParamPackImpl<ParamImplTypes...>>;

                        explicit ParamPackImpl(typename std::unique_ptr<ParamImplTypes> &&... ptrs)
                        {
                            SavePtrs(0, std::move(ptrs)...);
                        }

                        ~ParamPackImpl()
                        {
                        }

                        size_t GetNumParameters() const override
                        {
                            return NUM_PARAMS;
                        }

                        const bbox::enc::api::Param &GetParameter(size_t index) const override
                        {
                            BBOX_ASSERT(index < NUM_PARAMS);

                            return *m_param_ptrs[index];
                        }

                        bbox::TypeInfo GetTupleType() const override
                        {
                            return typeid(ValuesTupleType);
                        }

                        bool IsTupleCorrectType(const boost::any &tuple) const override
                        {
                            return tuple.type() == typeid(ValuesTupleType);
                        }

                        boost::any CreateDefaultConstructedTuple() const override
                        {
                            return ValuesTupleType();
                        }

                        std::string TupleToDebugString(bool for_input, const boost::any &tuple) const override
                        {
                            bbox::enc::ToJson to_json(ToTextFormat::HUMAN_DEBUGGING);
                            TupleToTextFormat(to_json, for_input, tuple);

                            return to_json.ToString();
                        }

                        void TupleToBinary(ToBinary &m, bool for_input, const boost::any &tuple) const override
                        {
                            BBOX_ASSERT(tuple.type() == typeid(ValuesTupleType));
                            const ValuesTupleType &cast_tuple = *boost::any_cast<ValuesTupleType>(&tuple);

                            Unpack<0, ParamPackImpl<ParamImplTypes...>>::ToBinary(m, for_input, cast_tuple);
                        }

                        void TupleToTextFormat(ToTextFormat &m, bool for_input, const boost::any &tuple) const override
                        {
                            BBOX_ASSERT(tuple.type() == typeid(ValuesTupleType));
                            const ValuesTupleType &cast_tuple = *boost::any_cast<ValuesTupleType>(&tuple);

                            m.StartStructure();
                            Unpack<0, ParamPackImpl<ParamImplTypes...>>::ToTextFormat(m, for_input, m_param_ptrs.data(), cast_tuple);
                            m.CompleteStructure();
                        }

                        void TupleFromBinary(FromBinary &m, bool for_input, boost::any &tuple) const override
                        {
                            BBOX_ASSERT(tuple.type() == typeid(ValuesTupleType));
                            ValuesTupleType &cast_tuple = *boost::any_cast<ValuesTupleType>(&tuple);

                            Unpack<0, ParamPackImpl<ParamImplTypes...>>::FromBinary(m, for_input, cast_tuple);
                        }

                        void TupleFromTextFormat(FromTextFormat &m, bool for_input, boost::any &tuple) const
                        {
                            BBOX_ASSERT(tuple.type() == typeid(ValuesTupleType));
                            ValuesTupleType &cast_tuple = *boost::any_cast<ValuesTupleType>(&tuple);

                            m.StartStructure();
                            Unpack<0, ParamPackImpl<ParamImplTypes...>>::FromTextFormat(m, for_input, m_param_ptrs.data(), cast_tuple);
                            m.CompleteStructure();
                        }

                        template <typename... Arguments>
                        struct CheckArgumentTypes
                        {
                            static constexpr bool value = All<CheckFunctionArgument<Arguments, ParamImplTypes>...>::value;

                            static_assert(value == true, "Invalid argument types for C++ function passed to method constructor.");
                        };

                    private:

                        std::array<std::unique_ptr<bbox::enc::api::Param>, NUM_PARAMS> m_param_ptrs;
                    };

                } // namespace bbox::enc::api::describe::details
            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE__PARAM_PACK_IMPL_H__
