/**
 * @file
 *
 * Header file for the bbox::enc::details::ToTextFormatAction template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__TO_TEXT_FORMAT_ACTION_H__
#define __BBOX__ENC__DETAILS__TO_TEXT_FORMAT_ACTION_H__

#include <bbox/enc/MsgPtr.h>

namespace bbox
{
    namespace enc
    {
        namespace details
        {

            template <>
            struct ToTextFormatAction<MarshalStrategy::AsStdString, std::string>
            {
                static void Impl(ToTextFormat &m, const std::string &value)
                {
                    m.SetStringValue(value);
                }
            };

            template <>
            struct ToTextFormatAction<MarshalStrategy::AsBool, bool>
            {
                static void Impl(ToTextFormat &m, const bool &value)
                {
                    m.SetStringValue(bbox::ToString(value));
                }
            };

            template <typename Type>
            struct ToTextFormatAction<MarshalStrategy::AsSignedInt, Type>
            {
                static void Impl(ToTextFormat &m, const Type &value)
                {
                    m.SetStringValue(bbox::ToString(value));
                }
            };

            template <typename Type>
            struct ToTextFormatAction<MarshalStrategy::AsUnsignedInt, Type>
            {
                static void Impl(ToTextFormat &m, const Type &value)
                {
                    m.SetStringValue(bbox::ToString(value));
                }
            };

            template <>
            struct ToTextFormatAction<MarshalStrategy::AsBinaryVector, std::vector<uint8_t>>
            {
                static void Impl(ToTextFormat &m, const std::vector<uint8_t> &value)
                {
                    m.SetBinaryDataValue(value);
                }
            };

            template <typename Type>
            struct ToTextFormatAction<MarshalStrategy::ViaCustomMethods, Type>
            {
                static void Impl(ToTextFormat &m, const Type &value)
                {
                    value.ToTextFormat(m);
                }
            };

            template <typename Type>
            struct ToTextFormatAction<MarshalStrategy::ViaBinaryCustomMethodsAndToFromString, Type>
            {
                static void Impl(ToTextFormat &m, const Type &value)
                {
                    m.SetStringValue(value.ToString());
                }
            };

            template <typename PairType>
            struct ToTextFormatAction<MarshalStrategy::AsStdPair, PairType>
            {
                static void Impl(ToTextFormat &m, const PairType &value)
                {
                    m.StartStructure();
                    m.AddNamedValue("first", value.first);
                    m.AddNamedValue("second", value.second);
                    m.CompleteStructure();
                }
            };

            template <typename Type, typename Allocator>
            struct ToTextFormatAction<MarshalStrategy::AsStdVector, std::vector<Type, Allocator>>
            {
                static void Impl(ToTextFormat &m, const std::vector<Type, Allocator> &value)
                {
                    m.StartStructure();
                    m.AddNamedValue("size", bbox::ToString(value.size()));
                    for (size_t index = 0; index < value.size(); ++index)
                    {
                        m.AddNamedValue("entry", value[index]);
                    }
                    m.CompleteStructure();
                }
            };

            template <typename Type, typename Comparator, typename Allocator>
            struct ToTextFormatAction<MarshalStrategy::AsStdSet, std::set<Type, Comparator, Allocator>>
            {
                static void Impl(ToTextFormat &m, const std::set<Type, Comparator, Allocator> &value)
                {
                    m.StartStructure();
                    m.AddNamedValue("size", bbox::ToString(value.size()));
                    for (const auto &entry : value)
                    {
                        m.AddNamedValue("entry", entry);
                    }
                    m.CompleteStructure();
                }
            };

            template <typename MapType>
            struct ToTextFormatAction<MarshalStrategy::AsStdMap, MapType>
            {
                static void Impl(ToTextFormat &m, const MapType &value)
                {
                    m.StartStructure();
                    m.AddNamedValue("size", bbox::ToString(value.size()));
                    for (const auto &entry : value)
                    {
                        m.StartNamedItem("entry");
                        m.StartStructure();
                        m.AddNamedValue("key", entry.first);
                        m.AddNamedValue("value", entry.second);
                        m.CompleteStructure();
                        m.CompleteNamedItem();
                    }
                    m.CompleteStructure();
                }
            };

			template <>
			struct ToTextFormatAction<MarshalStrategy::AsMsgPtr, MsgAnyPtr>
			{
				static void Impl(ToTextFormat &m, const MsgAnyPtr &value)
				{
					m.SetMsgAnyPtrValue(value);
				}
			};

			template <typename Type>
			struct ToTextFormatAction<MarshalStrategy::AsMsgPtr, MsgPtr<Type>>
			{
				static void Impl(ToTextFormat &m, const MsgPtr<Type> &value)
				{
					m.SetMsgAnyPtrValue(value);
				}
			};

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__TO_TEXT_FORMAT_ACTION_H__
