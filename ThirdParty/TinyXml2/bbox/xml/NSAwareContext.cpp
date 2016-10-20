/**
* @file
*
* Implementation for the bbox::xml::NSAwareContext class.
*/

#include <bbox/xml/NSAwareContext.h>

namespace bbox {
    namespace xml {

        NSAwareContext::Maps::Maps()
        {
        }

        NSAwareContext::Maps::~Maps()
        {
        }

        NSAwareContext::Maps::Maps(const Maps &copy)
            : ns_to_prefix(copy.ns_to_prefix)
            , prefix_to_ns(copy.prefix_to_ns)
        {
        }

        NSAwareContext::NSAwareContext(tinyxml2::XMLDocument &doc)
            : m_document(doc)
        {
        }

        NSAwareContext::~NSAwareContext()
        {
        }

    } // namespace bbox::xml
} // namespace bbox
