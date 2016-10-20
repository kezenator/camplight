/**
 * @file
 *
 * Header file for the bbox::enc::FromXml class.
 *
 */

#include <bbox/enc/FromXml.h>
#include <bbox/xml/Error.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

namespace bbox
{
    namespace enc
    {

        FromXml::FromXml(const std::string &xml)
        {
            bbox::Error error = m_doc.Parse(xml.c_str(), xml.size());

            if (error)
            {
                SetError(bbox::Format("Could not decode XML: %s", error.ToString()));
            }
            else
            {
                m_cur_element = nullptr;
                m_value_mode = false;
                m_first_in_structure = true;
            }
        }

        FromXml::~FromXml()
        {
        }

        size_t FromXml::EstimateNumNamedItemsInStructure()
        {
            if (HasError())
                return false;

            if (m_value_mode)
                return false;

            tinyxml2::XMLNode *parent_to_count;

            if (!m_cur_element)
            {
                if (m_first_in_structure)
                    parent_to_count = &m_doc;
                else
                    parent_to_count = nullptr;
            }
            else
            {
                if (m_first_in_structure)
                    parent_to_count = m_cur_element;
                else
                    parent_to_count = m_cur_element->Parent();
            }

            if (!parent_to_count)
                return 0;

            size_t result = 0;

            tinyxml2::XMLElement *child_ptr = parent_to_count->FirstChildElement();

            while (child_ptr)
            {
                ++result;

                child_ptr = child_ptr->NextSiblingElement();
            }

            return result;
        }

        bool FromXml::HasNextNamedItem(const char *name)
        {
            if (HasError())
                return false;

            if (m_value_mode)
                return false;

            tinyxml2::XMLElement *next_el_ptr;

            if (!m_cur_element)
            {
                if (m_first_in_structure)
                    next_el_ptr = m_doc.FirstChildElement();
                else
                    next_el_ptr = nullptr;
            }
            else
            {
                if (m_first_in_structure)
                    next_el_ptr = m_cur_element->FirstChildElement();
                else
                    next_el_ptr = m_cur_element->NextSiblingElement();
            }

            if (!next_el_ptr)
                return false;

            return 0 == strcmp(name, next_el_ptr->Name());
        }

        void FromXml::StartNamedItem(const char *name)
        {
            if (HasError())
                return;

            if (m_value_mode)
            {
                SetError("Cannot start named item in value mode - need to call StartStructure first");
                return;
            }

            tinyxml2::XMLElement *next_el_ptr;

            if (!m_cur_element)
            {
                if (m_first_in_structure)
                    next_el_ptr = m_doc.FirstChildElement();
                else
                    next_el_ptr = nullptr;
            }
            else
            {
                if (m_first_in_structure)
                    next_el_ptr = m_cur_element->FirstChildElement();
                else
                    next_el_ptr = m_cur_element->NextSiblingElement();
            }

            if (!next_el_ptr)
            {
                SetError(bbox::Format("No next element searching for \"%s\"", name));
                return;
            }

            if (0 != strcmp(name, next_el_ptr->Name()))
            {
                SetError(bbox::Format("Found next element \"%s\" but expecting \"%s\"", next_el_ptr->Name(), name));
                return;
            }

            m_cur_element = next_el_ptr;
            m_value_mode = true;
            m_first_in_structure = false;
        }

        void FromXml::CompleteNamedItem()
        {
            if (HasError())
                return;

            if (!m_value_mode)
            {
                SetError("Called CompleteNamedItem when not in value mode");
                return;
            }

            // Now that we've completed this named value,
            // we move back to structure mode, and we can't
            // be the first item in the structure anymore

            m_value_mode = false;
            m_first_in_structure = false;
        }

        void FromXml::StartStructure()
        {
            if (HasError())
                return;

            if (!m_value_mode)
            {
                SetError("StartStructure called when not in value mode");
                return;
            }

            m_value_mode = false;
            m_first_in_structure = true;
        }

        void FromXml::CompleteStructure()
        {
            if (HasError())
                return;

            if (m_value_mode)
            {
                SetError("CompleteStructure call in value mode");
                return;
            }

            BBOX_ASSERT(m_cur_element);

            // When the structure is complete
            // return back to value mode at the parent level

            if (m_first_in_structure)
            {
                // It's been an empty structure - we've not
                // descended into a child element - so no need
                // to pop up to a parent

                tinyxml2::XMLElement *remaining_el = m_cur_element->FirstChildElement();

                if (remaining_el)
                {
                    SetError(bbox::Format("Undecoded element \"%s\" remaining in structure",
                        remaining_el->Name()));
                    return;
                }
            }
            else
            {
                tinyxml2::XMLElement *remaining_el = m_cur_element->NextSiblingElement();

                if (remaining_el)
                {
                    SetError(bbox::Format("Undecoded element \"%s\" remaining in structure",
                        remaining_el->Name()));
                    return;
                }

                m_cur_element = m_cur_element->Parent()->ToElement();
            }

            m_value_mode = true;
            m_first_in_structure = false;
        }

        std::string FromXml::DecodeStringValue()
        {
            std::string result;

            if (HasError())
            {
                // Already has an error raised
            }
            else if (!m_value_mode)
            {
                SetError("DecodeValue called when not in value mode");
            }
            else
            {
                const char *attr_ptr = m_cur_element->Attribute("value");

                if (!attr_ptr)
                {
                    SetError("No value attribute for value element");
                }
                else
                {
                    result = attr_ptr;
                }
            }

            return result;
        }

        void FromXml::EnsureAllDecoded()
        {
            if (HasError())
                return;

            if (!m_value_mode
                && (m_cur_element == m_doc.FirstChildElement()))
            {
                // All OK
            }
            else
            {
                SetError("Undecoded data remaining");
            }
        }

    } // namespace bbox::enc
} // namespace bbox
