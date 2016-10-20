/**
* @file
*
* Header for the bbox::xml::NSAwareElement class.
*/

#ifndef __BBOX__XML__NS_AWARE_ELEMENT_H__
#define __BBOX__XML__NS_AWARE_ELEMENT_H__

#pragma once

#include <bbox/xml/NSAwareContext.h>

namespace bbox {
    namespace xml {

        /**
         * A sub-context that manages the namespaces for an element.
         */
        class NSAwareElement
        {
            NSAwareElement() = delete;
            NSAwareElement(const NSAwareElement &) = delete;
            void operator =(const NSAwareElement &) = delete;

        public:
            NSAwareElement(NSAwareContext &context, tinyxml2::XMLElement *element);
            NSAwareElement(NSAwareElement &parent, tinyxml2::XMLElement *element);
            ~NSAwareElement();

            std::string GetNamespace(const char *tagged);
            std::string GetName(const char *tagged);
            std::string GetTagged(const char *ns, const char *name);
            bool Matches(const char *tagged, const char *ns, const char *name);

            bool ElementNameMatches(const char *ns, const char *name);

            bool HasAttribute(const char *ns, const char *name);
            std::string GetAttribute(const char *ns, const char *name);

        private:

            void ParseElement();
            void AddMapping(const std::string &prefix, const std::string &ns);

            typedef NSAwareContext::Maps Maps;
            typedef NSAwareContext::MapsPtr MapsPtr;

            NSAwareContext &m_context;
            const tinyxml2::XMLElement *m_element_ptr;
            MapsPtr m_maps_ptr;
        };

    } // namespace bbox::xml
} // namespace bbox

#endif // __BBOX__XML__NS_AWARE_ELEMENT_H__