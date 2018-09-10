/**
 * @file
 *
 * Implementation file for the bbox::enc::ToTextFormat class.
 *
 */

#include <bbox/enc/ToTextFormat.h>
#include <bbox/Base64.h>
#include <bbox/Base16.h>
#include <bbox/Format.h>

namespace bbox
{
    namespace enc
    {

        ToTextFormat::ToTextFormat(E_TARGET target /* = MACHINE_ROUND_TRIP */)
            : m_target(target)
        {
        }

        ToTextFormat::~ToTextFormat()
        {
        }

        void ToTextFormat::SetStringValue(const std::string &canonical, const std::string &human_readable)
        {
            if (m_target == HUMAN_DEBUGGING)
            {
                SetStringValue(human_readable);
            }
            else
            {
                SetStringValue(canonical);
            }
        }

        void ToTextFormat::SetBinaryDataValue(const std::vector<uint8_t> &data)
        {
            if (m_target == HUMAN_DEBUGGING)
            {
                std::string result = bbox::Format("Binary data, %d bytes", data.size());

                if (data.size() == 0)
                {
                    // Leave it
                }
                else if (data.size() <= 20)
                {
                    result = bbox::Format("%s, %s", result, Base16::Encode(data));
                }
                else
                {
                    std::vector<uint8_t> shorter(&data[0], &data[20]);
                    result = bbox::Format("%s, %s...", result, Base16::Encode(shorter));
                }

                SetStringValue(result);
            }
            else
            {
                SetStringValue(Base64::Encode(data));
            }
        }

		void ToTextFormat::SetMsgAnyPtrValue(const MsgAnyPtr &val)
		{
			StartStructure();
			if (!val)
			{
				AddNamedValue("type", std::string("nullptr"));
			}
			else
			{
				AddNamedValue("type", val.GetType().GetName());
				StartNamedItem("contents");
				val.m_value->EncodeContents(*this);
				CompleteNamedItem();
			}
			CompleteStructure();
		}

    } // namespace bbox::enc
} // namespace bbox
