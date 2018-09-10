/**
 * @file
 *
 * Header file for the bbox::enc::details::ToBinaryAction template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__TO_BINARY_ACTION_H__
#define __BBOX__ENC__DETAILS__TO_BINARY_ACTION_H__

namespace bbox
{
    namespace enc
    {
        namespace details
        {

            template <>
            struct ToBinaryAction<MarshalStrategy::AsStdString, std::string>
            {
                static void Impl(ToBinary &m, const std::string &value)
                {
                    m.WriteString(value);
                }
            };

            template <>
            struct ToBinaryAction<MarshalStrategy::AsBool, bool>
            {
                static void Impl(ToBinary &m, const bool &value)
                {
                    m.WriteBool(value);
                }
            };

            template <typename Type>
            struct ToBinaryAction<MarshalStrategy::AsSignedInt, Type>
            {
                static void Impl(ToBinary &m, const Type &value)
                {
                    m.WriteIntByType(value);
                }
            };

            template <typename Type>
            struct ToBinaryAction<MarshalStrategy::AsUnsignedInt, Type>
            {
                static void Impl(ToBinary &m, const Type &value)
                {
                    m.WriteIntByType(value);
                }
            };

            template <>
            struct ToBinaryAction<MarshalStrategy::AsBinaryVector, std::vector<uint8_t>>
            {
                static void Impl(ToBinary &m, const std::vector<uint8_t> &value)
                {
                    m.WriteByteVector(value);
                }
            };

            template <typename Type>
            struct ToBinaryAction<MarshalStrategy::ViaCustomMethods, Type>
            {
                static void Impl(ToBinary &m, const Type &value)
                {
                    value.ToBinary(m);
                }
            };

            template <typename Type>
            struct ToBinaryAction<MarshalStrategy::ViaBinaryCustomMethodsAndToFromString, Type>
            {
                static void Impl(ToBinary &m, const Type &value)
                {
                    value.ToBinary(m);
                }
            };

            template <typename PairType>
            struct ToBinaryAction<MarshalStrategy::AsStdPair, PairType>
            {
                static void Impl(ToBinary &m, const PairType &value)
                {
                    m.Write(value.first);
                    m.Write(value.second);
                }
            };

            template <typename Type, typename Allocator>
            struct ToBinaryAction<MarshalStrategy::AsStdVector, std::vector<Type, Allocator>>
            {
                static void Impl(ToBinary &m, const std::vector<Type, Allocator> &value)
                {
                    size_t count = value.size();

                    m.WriteSizeT(count);

                    for (size_t index = 0; index < count; ++index)
                    {
                        m.Write(value[index]);
                    }
                }
            };

            template <typename Type, typename Comparator, typename Allocator>
            struct ToBinaryAction<MarshalStrategy::AsStdSet, std::set<Type, Comparator, Allocator>>
            {
                static void Impl(ToBinary &m, const std::set<Type, Comparator, Allocator> &value)
                {
                    size_t count = value.size();

                    m.WriteSizeT(count);

                    for (const auto &entry : value)
                    {
                        m.Write(entry);
                    }
                }
            };

            template <typename MapType>
            struct ToBinaryAction<MarshalStrategy::AsStdMap, MapType>
            {
                static void Impl(ToBinary &m, const MapType &value)
                {
                    m.WriteSizeT(value.size());

                    for (const auto &entry : value)
                    {
                        m.Write(entry.first);
                        m.Write(entry.second);
                    }
                }
            };

			template <>
			struct ToBinaryAction<MarshalStrategy::AsMsgPtr, MsgAnyPtr>
			{
				static void Impl(ToBinary &m, const MsgAnyPtr &value)
				{
					m.WriteMsgAnyPtr(value);
				}
			};

			template <typename Type>
			struct ToBinaryAction<MarshalStrategy::AsMsgPtr, MsgPtr<Type>>
			{
				static void Impl(ToBinary &m, const MsgPtr<Type> &value)
				{
					m.WriteMsgAnyPtr(value);
				}
			};

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__TO_BINARY_ACTION_H__
