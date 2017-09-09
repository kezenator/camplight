/**
 * @file
 *
 * Header file for the bbox::enc::details::FromBinaryAction template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__FROM_BINARY_ACTION_H__
#define __BBOX__ENC__DETAILS__FROM_BINARY_ACTION_H__

namespace bbox
{
    namespace enc
    {
        namespace details
        {

            template <>
            struct FromBinaryAction<MarshalStrategy::AsStdString, std::string>
            {
                static void Impl(FromBinary &m, std::string &value)
                {
                    value = m.ReadString();
                }
            };

            template <>
            struct FromBinaryAction<MarshalStrategy::AsBool, bool>
            {
                static void Impl(FromBinary &m, bool &value)
                {
                    value = m.ReadBool();
                }
            };

            template <typename Type>
            struct FromBinaryAction<MarshalStrategy::AsSignedInt, Type>
            {
                static void Impl(FromBinary &m, Type &value)
                {
                    m.ReadIntByType(value);
                }
            };

            template <typename Type>
            struct FromBinaryAction<MarshalStrategy::AsUnsignedInt, Type>
            {
                static void Impl(FromBinary &m, Type &value)
                {
                    m.ReadIntByType(value);
                }
            };

            template <>
            struct FromBinaryAction<MarshalStrategy::AsBinaryVector, std::vector<uint8_t>>
            {
                static void Impl(FromBinary &m, std::vector<uint8_t> &value)
                {
                    value = m.ReadByteVector();
                }
            };

            template <typename Type>
            struct FromBinaryAction<MarshalStrategy::ViaCustomMethods, Type>
            {
                static void Impl(FromBinary &m, Type &value)
                {
                    value.FromBinary(m);
                }
            };

            template <typename Type>
            struct FromBinaryAction<MarshalStrategy::ViaBinaryCustomMethodsAndToFromString, Type>
            {
                static void Impl(FromBinary &m, Type &value)
                {
                    value.FromBinary(m);
                }
            };

            template <typename PairType>
            struct FromBinaryAction<MarshalStrategy::AsStdPair, PairType>
            {
                static void Impl(FromBinary &m, PairType &value)
                {
                    m.Read(value.first);
                    m.Read(value.second);
                }
            };

            template <typename Type, typename Allocator>
            struct FromBinaryAction<MarshalStrategy::AsStdVector, std::vector<Type, Allocator>>
            {
                static void Impl(FromBinary &m, std::vector<Type, Allocator> &value)
                {
                    size_t count = m.ReadSizeT();

                    value.clear();
                    value.resize(count);

                    for (size_t index = 0; index < count; ++index)
                    {
                        m.Read(value[index]);
                    }
                }
            };

            template <typename Type, typename Comparator, typename Allocator>
            struct FromBinaryAction<MarshalStrategy::AsStdSet, std::set<Type, Comparator, Allocator>>
            {
                static void Impl(FromBinary &m, std::set<Type, Comparator, Allocator> &value)
                {
                    size_t count = m.ReadSizeT();

                    value.clear();

                    for (size_t i = 0; i < count; ++i)
                    {
                        Type new_value;
                        m.Read(new_value);

                        auto insert_result = value.insert(std::move(new_value));

                        if (!insert_result.second)
                        {
                            m.SetError("Duplicate value in std::set");
                        }
                    }
                }
            };

            template <typename MapType>
            struct FromBinaryAction<MarshalStrategy::AsStdMap, MapType>
            {
                static void Impl(FromBinary &m, MapType &value)
                {
                    using KeyType = typename MapType::key_type;
                    using MappedType = typename MapType::mapped_type;

                    size_t count = m.ReadSizeT();

                    value.clear();

                    for (size_t index = 0; index < count; ++index)
                    {
                        KeyType key;
                        MappedType mapped;

                        m.Read(key);
                        m.Read(mapped);

                        if (m.HasError())
                            break;

                        auto insert_result = value.emplace(std::move(key), std::move(mapped));

                        if (!insert_result.second)
                        {
                            m.SetError("Duplicate key in std::map FromBinary");
                            break;
                        }
                    }
                }
            };

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__FROM_BINARY_ACTION_H__
