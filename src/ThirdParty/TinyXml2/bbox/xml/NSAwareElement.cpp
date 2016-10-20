/**
* @file
*
* Implementation for the bbox::xml::NSAwareElement class.
*/

#include <bbox/xml/NSAwareElement.h>

namespace bbox {
    namespace xml {

        NSAwareElement::NSAwareElement(NSAwareContext &context, tinyxml2::XMLElement *element)
            : m_context(context)
            , m_element_ptr(element)
            , m_maps_ptr(new Maps())
        {
            ParseElement();
        }

        NSAwareElement::NSAwareElement(NSAwareElement &parent, tinyxml2::XMLElement *element)
            : m_context(parent.m_context)
            , m_element_ptr(element)
            , m_maps_ptr(parent.m_maps_ptr)
        {
            ParseElement();
        }

        NSAwareElement::~NSAwareElement()
        {
        }

        void NSAwareElement::ParseElement()
        {
            // Go through all the attributes and process xmlns ones -
            // ensuring we create a new copy of the maps if any changes
            // are required. Remove any old mapping from the prefix to namespace
            // from the namespace to previx map, then add the new mappings.

            for (const tinyxml2::XMLAttribute *attr_ptr = m_element_ptr->FirstAttribute();
                 attr_ptr != nullptr;
                 attr_ptr = attr_ptr->Next())
            {
                if (strcmp(attr_ptr->Name(), "xmlns") == 0)
                {
                    // We've got a new default element

                    AddMapping(std::string(), attr_ptr->Value());
                }
                else if (strncmp(attr_ptr->Name(), "xmlns:", 6) == 0)
                {
                    AddMapping(attr_ptr->Name() + 6, attr_ptr->Value());
                }
            }
        }

        void NSAwareElement::AddMapping(const std::string &prefix, const std::string &ns)
        {
            if (!m_maps_ptr.unique())
                m_maps_ptr = MapsPtr(new Maps(*m_maps_ptr));

            auto it = m_maps_ptr->prefix_to_ns.find(prefix);
            if (it != m_maps_ptr->prefix_to_ns.end())
                m_maps_ptr->ns_to_prefix.erase(it->second);

            m_maps_ptr->prefix_to_ns[prefix] = ns;
            m_maps_ptr->ns_to_prefix[ns] = prefix;
        }

        std::string NSAwareElement::GetNamespace(const char *tagged)
        {
            // Find the colon or end of string
            const char *div_ptr = tagged;
            while ((*div_ptr != 0) && (*div_ptr != ':'))
                ++div_ptr;

            std::string ns;

            if (*div_ptr == 0)
            {
                // There's no colon - use the default namespace
            }
            else
            {
                // Theres a colon - get the namespace
                // for the first section

                ns.assign(tagged, div_ptr - tagged);
            }

            auto it = m_maps_ptr->prefix_to_ns.find(ns);

            if (it == m_maps_ptr->prefix_to_ns.end())
                return std::string();
            else
                return it->second;
        }

        std::string NSAwareElement::GetName(const char *tagged)
        {
            // Find the colon or end of string
            const char *div_ptr = tagged;
            while ((*div_ptr != 0) && (*div_ptr != ':'))
                ++div_ptr;

            if (*div_ptr == 0)
            {
                // There's no colon - everything is the name
                return tagged;
            }
            else
            {
                // Theres a colon - the name is afterwards

                return div_ptr + 1;
            }
        }

        std::string NSAwareElement::GetTagged(const char *ns, const char *name)
        {
            auto it = m_maps_ptr->ns_to_prefix.find(ns);

            if (it == m_maps_ptr->ns_to_prefix.end())
            {
                // The namespace isn't mapped - this is really
                // an error - but we'll just return the name
                return name;
            }

            std::string result(it->second);
            result.push_back(':');
            result.append(name);

            return result;
        }

        bool NSAwareElement::Matches(const char *tagged, const char *ns, const char *name)
        {
            return GetTagged(ns, name) == tagged;
        }

        bool NSAwareElement::ElementNameMatches(const char *ns, const char *name)
        {
            return Matches(m_element_ptr->Name(), ns, name);
        }

        bool NSAwareElement::HasAttribute(const char *ns, const char *name)
        {
            std::string tagged = GetTagged(ns, name);

            return m_element_ptr->FindAttribute(tagged.c_str()) != 0;
        }

        std::string NSAwareElement::GetAttribute(const char *ns, const char *name)
        {
            std::string tagged = GetTagged(ns, name);

            const tinyxml2::XMLAttribute *attr_ptr
                = m_element_ptr->FindAttribute(tagged.c_str());

            if (attr_ptr)
                return attr_ptr->Value();
            else
                return std::string();
        }

    } // namespace bbox::xml
} // namespace bbox
