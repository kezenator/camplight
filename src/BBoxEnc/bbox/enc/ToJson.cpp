/**
 * @file
 *
 * Implementation file for the bbox::enc::ToDebugString class.
 *
 */

#include <bbox/enc/ToJson.h>
#include <bbox/Format.h>

namespace bbox
{
    namespace enc
    {

        ToJson::ToJson(E_TARGET target /* = MACHINE_ROUND_TRIP */)
            : ToTextFormat(target)
        {
        }

        ToJson::~ToJson()
        {
        }

        void ToJson::StartNamedItem(const char *name)
        {
            if (!m_first_in_structure.empty())
            {
                if (m_first_in_structure.back())
                    m_first_in_structure.back() = false;
                else
                {
                    m_stream << ',';

                    if (GetTarget() == HUMAN_DEBUGGING)
                    {
                        // Include an extra space in human debugging mode
                        m_stream << ' ';
                    }
                }
            }

            if (GetTarget() == HUMAN_DEBUGGING)
            {
                // Dont include quotes on the name for
                // human debugging format as I think they are confusing

                m_stream << name << ':';
            }
            else
            {
                // For offical round-trip JSON, the name needs quotes
                m_stream << '\"' << name << "\":";
            }
        }

        void ToJson::CompleteNamedItem()
        {
        }

        void ToJson::StartStructure()
        {
            m_first_in_structure.push_back(true);
            m_stream << "{";
        }

        void ToJson::CompleteStructure()
        {
            if (!m_first_in_structure.empty())
                m_first_in_structure.pop_back();
            m_stream << "}";
        }

        void ToJson::SetStringValue(const std::string &canonical)
        {
            m_stream << '\"' << EscapeString(canonical) << '\"';
        }

        std::string ToJson::EscapeString(const std::string &val)
        {
            std::stringstream stream;

            for (char ch : val)
            {
                switch (ch)
                {
                case '\\':
                    stream << '\\' << '\\';
                    break;

                case '\"':
                    stream << '\\' << '\"';
                    break;

                case '\b':
                    stream << '\\' << 'b';
                    break;

                case '\f':
                    stream << '\\' << 'f';
                    break;

                case '\n':
                    stream << '\\' << 'n';
                    break;

                case '\r':
                    stream << '\\' << 'r';
                    break;

                case '\t':
                    stream << '\\' << 't';
                    break;

                default:
                    if ((ch < ' ')
                        || (ch > '~'))
                    {
                        stream << bbox::Format("\\u00%02X", ch);
                    }
                    else
                    {
                        stream << ch;
                    }
                    break;
                }
            }

            return stream.str();
        }

    } // namespace bbox::enc
} // namespace bbox
