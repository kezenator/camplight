/**
 * @file
 *
 * Header file for the bbox::enc::api::describe::details::MethodImpl class.
 */

#ifndef __BBOX__ENC__API__DESCRIBE__METHOD_IMPL_H__
#define __BBOX__ENC__API__DESCRIBE__METHOD_IMPL_H__

#include <functional>
#include <bbox/Assert.h>
#include <bbox/enc/api/describe/details/TypeTraits.h>
#include <bbox/enc/api/describe/details/ParamPackImpl.h>
#include <bbox/enc/api/Method.h>

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

                    template <typename ParamPackImplType,
                              typename ReturnType,
                              typename... Arguments>
                    class MethodImpl : public Method
                    {
                        static_assert(IsParamPackImplType<ParamPackImplType>::value, "Third argument to bbox::enc::api::describe::Method must only be the result of a call to bbox::enc::api::describe::ParamPack");
                        static_assert(std::is_assignable<bbox::Error, ReturnType>::value, "The function return type must be assignable to bbox::Error");

                    public:
                        using FunctionType = std::function<ReturnType(Arguments...)>;

                        using ValuesTupleType = typename ParamPackImplType::ValuesTupleType;
                        static constexpr size_t NUM_PARAMS = ParamPackImplType::NUM_PARAMS;

                        MethodImpl(const char *name,
                                   const char *description,
                                   std::unique_ptr<ParamPackImplType> &&param_pack_ptr,
                                   FunctionType &&func)
                            : m_name(name)
                            , m_description(description)
                            , m_param_pack_ptr(std::move(param_pack_ptr))
                            , m_function(std::move(func))
                        {
                        }

                        ~MethodImpl()
                        {
                        }

                        const std::string &GetName() const override
                        {
                            return m_name;
                        }

                        const std::string &GetDescription() const override
                        {
                            return m_description;
                        }

                        const bbox::enc::api::ParamPack &GetParamPack() const override
                        {
                            return *m_param_pack_ptr;
                        }

                        bbox::Error Call(boost::any &params_tuple) const override
                        {
                            // TODO - get working in g++
                            //static_assert(ParamPackImplType::CheckArgumentTypes<Arguments...>::value, "Invalid function parameter arguments for parameter types");

                            if (!m_param_pack_ptr->IsTupleCorrectType(params_tuple))
                                return boost::system::errc::invalid_argument;

                            ValuesTupleType *cast_tuple_ptr = boost::any_cast<ValuesTupleType>(&params_tuple);

                            return Apply(m_function, *cast_tuple_ptr, std::make_index_sequence<NUM_PARAMS>());
                        }

                    private:
                        const std::string m_name;
                        const std::string m_description;
                        const std::unique_ptr<ParamPackImplType> m_param_pack_ptr;
                        const FunctionType m_function;
                    };

                } // namespace bbox::enc::api::describe::details
            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE__METHOD_IMPL_H__
