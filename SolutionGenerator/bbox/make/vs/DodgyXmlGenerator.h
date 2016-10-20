/**
 * @file
 *
 * Header file for the bbox::make::vs::DodgyXmlGenerator class.
 */

#ifndef __BBOX__MAKE__VS__DODGY_XML_GENERATOR_H__
#define __BBOX__MAKE__VS__DODGY_XML_GENERATOR_H__

#pragma once

#include <string>
#include <list>
#include <sstream>

namespace bbox {
    namespace make {
        namespace vs {

            class DodgyXmlGenerator
            {
            public:
                DodgyXmlGenerator();
                ~DodgyXmlGenerator();

                std::string str()
                {
                    return m_stream.str();
                }

                void StartElement(const std::string &name);
                void SetAttribute(const std::string &name, const std::string &value);
                void SetText(const std::string &text);
                void CloseElement();

                void SetTextElement(const std::string &name, const std::string &value)
                {
                    StartElement(name);
                    SetText(value);
                    CloseElement();
                }

                class Element
                {
                    Element(const Element &) = delete;
                    void operator =(const Element &) = delete;

                public:
                    Element(DodgyXmlGenerator &doc, const std::string &name)
                        : m_doc(doc)
                    {
                        doc.StartElement(name);
                    }

                    ~Element()
                    {
                        m_doc.CloseElement();
                    }

                private:
                    DodgyXmlGenerator &m_doc;
                };

            private:

                bool FinishStart();

                std::stringstream m_stream;
                std::string m_indent;
                std::list<std::string> m_element_stack;
                bool m_in_element_start;
                bool m_set_text;
                bool m_set_sub_element;
            };

        } // namespace bbox::make::vs
    } // namesoace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__VS__DODGY_XML_GENERATOR_H__
