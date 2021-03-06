/**
 * @file
 *
 * Implementation file for the bbox::enc::FromTextFormat class.
 *
 */

#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/MsgAnyPtr.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/Base64.h>

namespace bbox
{
    namespace enc
    {

        FromTextFormat::FromTextFormat()
            : m_Error(false)
            , m_ErrorStr()
        {
        }

        FromTextFormat::~FromTextFormat()
        {
        }

        void FromTextFormat::SetError(const std::string &error_str)
        {
            if (!m_Error)
            {
                m_Error = true;
                m_ErrorStr = error_str;
            }
        }

        void FromTextFormat::SetError(std::string &&error_str)
        {
            if (!m_Error)
            {
                m_Error = true;
                m_ErrorStr = std::move(error_str);
            }
        }
        
        bool FromTextFormat::HasNextNamedItem(const std::string &name)
        {
          return HasNextNamedItem(name.c_str());
        }

        std::vector<uint8_t> FromTextFormat::DecodeBinaryDataValue()
        {
            std::vector<uint8_t> result;

            if (!HasError())
            {
                std::string base64 = DecodeStringValue();

                if (!HasError())
                {
                    bbox::Error error = bbox::Base64::Decode(base64, result);

                    if (error)
                    {
                        SetError("Un-decodeable base-64 binary data");
                    }
                }
            }

            return result;
        }

        void FromTextFormat::DecodeNamedSizeT(const char *name, size_t &value)
        {
            value = 0;

            if (!HasError())
            {
                uint64_t uint64_value;

                DecodeNamedValue(name, uint64_value);

                if ((uint64_value < std::numeric_limits<size_t>::min())
                    || (uint64_value > std::numeric_limits<size_t>::max()))
                {
                    SetError("Out-of-range size_t value");
                }
                else
                {
                    value = size_t(uint64_value);
                }
            }
        }

		MsgAnyPtr FromTextFormat::DecodeMsgAnyPtr()
		{
			MsgAnyPtr result;

			StartStructure();

			std::string type_name;
			DecodeNamedValue("type", type_name);

			if (type_name == "nullptr")
			{
				// All good
			}
			else
			{
				MsgType type = MsgTypeLibrary::FindByName(type_name);
				if (!type)
				{
					SetError(bbox::Format("Unknown MsgType \"%s\"", type_name));
				}
				else
				{
					result = type.CreateNewObject();

					StartNamedItem("contents");
					result.m_value->DecodeContents(*this);
					CompleteNamedItem();
				}
			}

			CompleteStructure();

			return result;
		}

    } // namespace bbox::enc
} // namespace bbox
