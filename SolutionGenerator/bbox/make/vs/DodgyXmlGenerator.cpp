/**
* @file
*
* Implementation file for the bbox::make::vs::DodgyXmlGenerator class.
*/

#include <bbox/make/vs/DodgyXmlGenerator.h>

namespace bbox {
    namespace make {
        namespace vs {

            DodgyXmlGenerator::DodgyXmlGenerator()
                : m_stream()
                , m_indent()
                , m_element_stack()
                , m_in_element_start(false)
                , m_set_text(false)
                , m_set_sub_element(false)
            {
                m_stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
            }

            DodgyXmlGenerator::~DodgyXmlGenerator()
            {
            }

            void
            DodgyXmlGenerator::StartElement(const std::string &name)
            {
                if (FinishStart())
                {
                    m_stream << std::endl;
                }

                m_stream << m_indent << "<" << name;

                m_in_element_start = true;
                m_set_text = false;
                m_set_sub_element = false;

                m_element_stack.push_back(name);
                m_indent.push_back(' ');
                m_indent.push_back(' ');
            }

            void
            DodgyXmlGenerator::SetAttribute(const std::string &name, const std::string &value)
            {
                m_stream << " " << name << "=\"" << value << "\"";
            }

            void
            DodgyXmlGenerator::SetText(const std::string &text)
            {
                FinishStart();
                
                m_stream << text;
                m_set_text = true;
            }

            void
            DodgyXmlGenerator::CloseElement()
            {
                m_indent.pop_back();
                m_indent.pop_back();

                if (m_in_element_start)
                {
                    m_stream << " />" << std::endl;
                }
                else
                {
                    if (!m_set_text && !m_set_sub_element)
                    {
                        m_stream << std::endl;
                    }
                    
                    if (!m_set_text)
                    {
                        m_stream << m_indent;
                    }

                    m_stream << "</" << m_element_stack.back() << ">" << std::endl;
                }

                m_element_stack.pop_back();

                m_in_element_start = false;
                m_set_text = false;
                m_set_sub_element = true;
            }

            bool
            DodgyXmlGenerator::FinishStart()
            {
                if (m_in_element_start)
                {
                    m_in_element_start = false;
                    m_set_text = false;
                    m_stream << ">";

                    return true;
                }

                return false;
            }

        } // namespace bbox::make::vs
    } // namesoace bbox::make
} // namespace bbox
