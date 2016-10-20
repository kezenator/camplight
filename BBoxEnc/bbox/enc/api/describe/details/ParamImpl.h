/**
 * @file
 *
 * Header file for the bbox::enc::api::describe::details::ParamImpl class.
 */

#ifndef __BBOX__ENC__API__DESCRIBE__PARAM_IMPL_H__
#define __BBOX__ENC__API__DESCRIBE__PARAM_IMPL_H__

#include <memory>

#include <bbox/enc/api/describe/details/TypeTraits.h>
#include <bbox/enc/api/Param.h>

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

                    template <typename _ValueType, typename _DirectionType>
                    class ParamImpl : public Param
                    {
                    public:
                        using ValueType = _ValueType;
                        using DirectionType = _DirectionType;
                        using Ptr = std::unique_ptr<ParamImpl<ValueType, DirectionType>>;

                        ParamImpl(std::string &&name, std::string &&description)
                            : m_name(std::move(name))
                            , m_description(std::move(description))
                        {
                        }

                        ~ParamImpl()
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

                        bbox::TypeInfo GetType() const override
                        {
                            return typeid(ValueType);
                        }

                        bool IsInput() const override
                        {
                            return DirectionType::IsInput;
                        }

                        bool IsOutput() const override
                        {
                            return DirectionType::IsOutput;
                        }

                    private:
                        const std::string m_name;
                        const std::string m_description;
                    };

                } // namespace bbox::enc::api::describe::details
            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE__PARAM_IMPL_H__
