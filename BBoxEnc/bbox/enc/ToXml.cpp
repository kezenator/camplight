/**
 * @file
 *
 * Implementation file for the bbox::enc::ToXml class.
 *
 */

#include <bbox/enc/ToXml.h>
#include <bbox/Assert.h>

namespace bbox
{
    namespace enc
    {

        ToXml::ToXml(tinyxml2::XMLDocument &doc, const char *root_element_name, E_TARGET target /* = MACHINE_ROUND_TRIP */)
            : ToTextFormat(target)
            , m_doc(doc)
            , m_cur_element(doc.NewElement(root_element_name))
        {
            m_doc.InsertEndChild(doc.NewDeclaration());
            m_doc.InsertEndChild(m_cur_element);
        }

        ToXml::~ToXml()
        {
        }

        void ToXml::StartNamedItem(const char *name)
        {
            tinyxml2::XMLElement *new_el = m_doc.NewElement(name);
            m_cur_element->InsertEndChild(new_el);
            m_cur_element = new_el;
        }

        void ToXml::CompleteNamedItem()
        {
            m_cur_element = m_cur_element->Parent()->ToElement();
        }

        void ToXml::StartStructure()
        {
            // Nothing to do - we'll just add
            // sub elements to the current element
        }

        void ToXml::CompleteStructure()
        {
            // Nothing to do
        }

        void ToXml::SetStringValue(const std::string &canonical)
        {
            m_cur_element->SetAttribute("value", canonical.c_str());
        }

    } // namespace bbox::enc
} // namespace bbox
