/**
 * @file
 *
 * Header file for the bbox::enc::details::FromTextFormatAction template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__FROM_TEXT_FORMAT_ACTION_H__
#define __BBOX__ENC__DETAILS__FROM_TEXT_FORMAT_ACTION_H__

#include <bbox/FromString.h>
#include <bbox/Format.h>
#include <bbox/TypeInfo.h>
#include <bbox/enc/MsgPtr.h>

namespace bbox
{
    namespace enc
    {
        namespace details
        {

            template <>
            struct FromTextFormatAction<MarshalStrategy::AsStdString, std::string>
            {
                static void Impl(FromTextFormat &m, std::string &value)
                {
                    value = m.DecodeStringValue();
                }
            };

            template <>
            struct FromTextFormatAction<MarshalStrategy::AsBool, bool>
            {
                static void Impl(FromTextFormat &m, bool &value)
                {
                    if (!m.HasError())
                    {
                        std::string to_decode = m.DecodeStringValue();

                        if (!bbox::FromString(to_decode, value))
                        {
                            m.SetError(bbox::Format("Invalid bool value \"%s\"", to_decode));
                        }
                    }
                }
            };

            template <typename Type>
            struct FromTextFormatAction<MarshalStrategy::AsSignedInt, Type>
            {
                static void Impl(FromTextFormat &m, Type &value)
                {
                    if (!m.HasError())
                    {
                        std::string to_decode = m.DecodeStringValue();

                        if (!bbox::FromString(to_decode, value))
                        {
                            m.SetError(bbox::Format("Invalid/out-of-range signed int value \"%s\"", to_decode));
                        }
                    }
                }
            };

            template <typename Type>
            struct FromTextFormatAction<MarshalStrategy::AsUnsignedInt, Type>
            {
                static void Impl(FromTextFormat &m, Type &value)
                {
                    if (!m.HasError())
                    {
                        std::string to_decode = m.DecodeStringValue();

                        if (!bbox::FromString(to_decode, value))
                        {
                            m.SetError(bbox::Format("Invalid/out-of-range unsigned int value \"%s\"", to_decode));
                        }
                    }
                }
            };

            template <>
            struct FromTextFormatAction<MarshalStrategy::AsBinaryVector, std::vector<uint8_t>>
            {
                static void Impl(FromTextFormat &m, std::vector<uint8_t> &value)
                {
                    value = m.DecodeBinaryDataValue();
                }
            };

            template <typename Type>
            struct FromTextFormatAction<MarshalStrategy::ViaCustomMethods, Type>
            {
                static void Impl(FromTextFormat &m, Type &value)
                {
                    value.FromTextFormat(m);
                }
            };

            template <typename Type>
            struct FromTextFormatAction<MarshalStrategy::ViaBinaryCustomMethodsAndToFromString, Type>
            {
                static void Impl(FromTextFormat &m, Type &value)
                {
                    if (!m.HasError())
                    {
                        std::string to_decode = m.DecodeStringValue();

                        if (!Type::FromString(to_decode, value))
                        {
                            m.SetError(bbox::Format("Invalid string \"%s\" for type %s",
                                to_decode,
                                bbox::TypeInfo::Of<Type>().pretty_name()));
                        }
                    }
                }
            };

            template <typename PairType>
            struct FromTextFormatAction<MarshalStrategy::AsStdPair, PairType>
            {
                static void Impl(FromTextFormat &m, PairType &value)
                {
                    m.StartStructure();
                    m.DecodeNamedValue("first", value.first);
                    m.DecodeNamedValue("second", value.second);
                    m.CompleteStructure();
                }
            };

            template <typename Type, typename Allocator>
            struct FromTextFormatAction<MarshalStrategy::AsStdVector, std::vector<Type, Allocator>>
            {
                static void Impl(FromTextFormat &m, std::vector<Type, Allocator> &value)
                {
                    value.clear();

                    m.StartStructure();
                    
                    size_t size;
                    m.DecodeNamedSizeT("size", size);

                    value.resize(size);

                    for (size_t i = 0; i < size; ++i)
                    {
                        m.DecodeNamedValue("entry", value[i]);
                    }

                    m.CompleteStructure();
                }
            };

			template <typename Allocator>
			struct FromTextFormatAction<MarshalStrategy::AsStdVector, std::vector<bool, Allocator>>
			{
				static void Impl(FromTextFormat &m, std::vector<bool, Allocator> &value)
				{
					value.clear();

					m.StartStructure();

					size_t size;
					m.DecodeNamedSizeT("size", size);

					value.resize(size);

					for (size_t i = 0; i < size; ++i)
					{
						bool entry;
						m.DecodeNamedValue("entry", entry);
						value[i] = entry;
					}

					m.CompleteStructure();
				}
			};

			template <typename Type, typename Comparator, typename Allocator>
            struct FromTextFormatAction<MarshalStrategy::AsStdSet, std::set<Type, Comparator, Allocator>>
            {
                static void Impl(FromTextFormat &m, std::set<Type, Comparator, Allocator> &value)
                {
                    value.clear();

                    m.StartStructure();

                    size_t size;
                    m.DecodeNamedSizeT("size", size);

                    for (size_t i = 0; i < size; ++i)
                    {
                        Type new_value;
                        m.DecodeNamedValue("entry", new_value);

                        auto insert_result = value.insert(std::move(new_value));

                        if (!insert_result.second)
                        {
                            m.SetError("Duplicate value in std::set");
                        }
                    }

                    m.CompleteStructure();
                }
            };

            template <typename MapType>
            struct FromTextFormatAction<MarshalStrategy::AsStdMap, MapType>
            {
                static void Impl(FromTextFormat &m, MapType &value)
                {
                    value.clear();

                    using KeyType = typename MapType::key_type;
                    using MappedType = typename MapType::mapped_type;

                    m.StartStructure();

                    size_t size;
                    m.DecodeNamedSizeT("size", size);

                    for (size_t i = 0; i < size; ++i)
                    {
                        m.StartNamedItem("entry");
                        m.StartStructure();

                        KeyType key;
                        MappedType mapped;

                        m.DecodeNamedValue("key", key);
                        m.DecodeNamedValue("value", mapped);

                        m.CompleteStructure();
                        m.CompleteNamedItem();

                        if (m.HasError())
                            break;

                        auto insert_result = value.emplace(std::move(key), std::move(mapped));

                        if (!insert_result.second)
                        {
                            m.SetError("Duplicate key in std::map FromTextFormat");
                            break;
                        }
                    }

                    m.CompleteStructure();
                }
            };

			template <>
			struct FromTextFormatAction<MarshalStrategy::AsMsgPtr, MsgAnyPtr>
			{
				static void Impl(FromTextFormat &m, MsgAnyPtr &value)
				{
					value = m.DecodeMsgAnyPtr();
				}
			};

			template <typename Type>
			struct FromTextFormatAction<MarshalStrategy::AsMsgPtr, MsgPtr<Type>>
			{
				static void Impl(FromTextFormat &m, MsgPtr<Type> &value)
				{
					MsgAnyPtr any_ptr = m.DecodeMsgAnyPtr();

					if (!m.HasError()
						&& any_ptr)
					{
						value = message_cast<Type>(any_ptr);

						if (!value)
						{
							m.SetError(bbox::Format("Could not cast object of type %s to type %s",
								any_ptr.GetType().GetName(),
								MsgTypeLibrary::FindByType<Type>().GetName()));
						}
					}
				}
			};

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__FROM_TEXT_FORMAT_ACTION_H__
