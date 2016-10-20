/**
* @file
*
* Header for the bbox::xml::ElementBuilder class.
*/

#ifndef __BBOX__XML__ELEMENT_BUILDER_H__
#define __BBOX__XML__ELEMENT_BUILDER_H__

#pragma once

#include <tinyxml2/tinyxml2.h>
#include <string>

namespace bbox {
    namespace xml {

        /**
         * Conveniance class for building tinyxml2 documents.
         */
        class ElementBuilder
        {
            ElementBuilder() = delete;
            ElementBuilder(const ElementBuilder &) = delete;
            void operator =(const ElementBuilder &) = delete;

        public:
            ElementBuilder(tinyxml2::XMLDocument &parent, const char *name)
                : m_element_ptr(parent.NewElement(name))
            {
                parent.InsertEndChild(m_element_ptr);
            }

            ElementBuilder(tinyxml2::XMLElement *parent, const char *name)
                : m_element_ptr(parent->GetDocument()->NewElement(name))
            {
                parent->InsertEndChild(m_element_ptr);
            }

            ElementBuilder(ElementBuilder &parent, const char *name)
                : m_element_ptr(parent.m_element_ptr->GetDocument()->NewElement(name))
            {
                parent.m_element_ptr->InsertEndChild(m_element_ptr);
            }

            ElementBuilder(tinyxml2::XMLDocument &parent, const std::string &name)
                : m_element_ptr(parent.NewElement(name.c_str()))
            {
                parent.InsertEndChild(m_element_ptr);
            }

            ElementBuilder(tinyxml2::XMLElement *parent, const std::string &name)
                : m_element_ptr(parent->GetDocument()->NewElement(name.c_str()))
            {
                parent->InsertEndChild(m_element_ptr);
            }

            ElementBuilder(ElementBuilder &parent, const std::string &name)
                : m_element_ptr(parent.m_element_ptr->GetDocument()->NewElement(name.c_str()))
            {
                parent.m_element_ptr->InsertEndChild(m_element_ptr);
            }

            tinyxml2::XMLElement *GetElementPtr()
            {
                return m_element_ptr;
            }

            void SetAttribute(const char *name, const char *value)
            {
                m_element_ptr->SetAttribute(name, value);
            }

            void SetAttribute(const char *name, const std::string &value)
            {
                m_element_ptr->SetAttribute(name, value.c_str());
            }

            void AddText(const char *value)
            {
                tinyxml2::XMLText *text = m_element_ptr->GetDocument()->NewText(value);
                m_element_ptr->InsertEndChild(text);
            }

            void AddText(const std::string &value)
            {
                tinyxml2::XMLText *text = m_element_ptr->GetDocument()->NewText(value.c_str());
                m_element_ptr->InsertEndChild(text);
            }

            void AddTextElement(const char *name, const char *value)
            {
                tinyxml2::XMLElement *element = m_element_ptr->GetDocument()->NewElement(name);
                m_element_ptr->InsertEndChild(element);
                element->SetText(value);
            }

            void AddTextElement(const char *name, const std::string &value)
            {
                AddTextElement(name, value.c_str());
            }

            void AddTextElement(const std::string &name, const std::string &value)
            {
                AddTextElement(name.c_str(), value.c_str());
            }

            void AddCdataElement(const char *name, const char *value)
            {
                tinyxml2::XMLElement *element = m_element_ptr->GetDocument()->NewElement(name);
                m_element_ptr->InsertEndChild(element);
                tinyxml2::XMLText *text = m_element_ptr->GetDocument()->NewText(value);
                text->SetCData(true);
                element->InsertEndChild(text);
            }

            void AddCdataElement(const char *name, const std::string &value)
            {
                AddCdataElement(name, value.c_str());
            }

            void AddCdataElement(const std::string &name, const std::string &value)
            {
                AddCdataElement(name.c_str(), value.c_str());
            }

        private:
            tinyxml2::XMLElement *m_element_ptr;
        };

    } // namespace bbox::xml
} // namespace bbox

#endif // __BBOX__XML__ELEMENT_BUILDER_H__