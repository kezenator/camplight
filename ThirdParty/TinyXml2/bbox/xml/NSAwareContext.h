/**
* @file
*
* Header for the bbox::xml::NSAwareContext class.
*/

#ifndef __BBOX__XML__NS_AWARE_CONTEXT_H__
#define __BBOX__XML__NS_AWARE_CONTEXT_H__

#pragma once

#include <map>
#include <memory>
#include <string>

#include <tinyxml2/tinyxml2.h>

namespace bbox {
    namespace xml {

        // Forward declaration
        class NSAwareElement;

        /**
         * Overall context for decoding XML files containing namespaces.
         */
        class NSAwareContext
        {
            friend class NSAwareElement;

            NSAwareContext() = delete;
            NSAwareContext(const NSAwareContext &) = delete;
            void operator =(const NSAwareContext &) = delete;

            class Maps
            {
            public:
                Maps();
                ~Maps();
                Maps(const Maps &copy);
                void operator =(const Maps &) = delete;

                std::map<std::string, std::string> ns_to_prefix;
                std::map<std::string, std::string> prefix_to_ns;
            };

            typedef std::shared_ptr<Maps> MapsPtr;

        public:
            NSAwareContext(tinyxml2::XMLDocument &doc);
            ~NSAwareContext();

        private:
            tinyxml2::XMLDocument &m_document;
        };

    } // namespace bbox::xml
} // namespace bbox

#endif // __BBOX__XML__NS_AWARE_CONTEXT_H__