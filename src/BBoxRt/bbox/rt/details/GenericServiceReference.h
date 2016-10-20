/**
 * @file
 *
 * Header for the bbox::rt::GeneriServiceReference class.
 */

#ifndef __BBOX__RT__DETAILS__GENERIC_SERVICE_REFERENCE_H__
#define __BBOX__RT__DETAILS__GENERIC_SERVICE_REFERENCE_H__

#pragma once

#include <string>

#include <bbox/rt/Resource.h>
#include <bbox/TypeInfo.h>

namespace bbox {
    namespace rt {

        // Forward declarations
        template <typename ServiceType>
        class ServiceReference;

        namespace details {

            /**
             * Base class for service references.
             */
            class GenericServiceReference: public Resource
            {
                friend class bbox::rt::Proactor;
                friend class bbox::rt::details::ResourceBase;

                template <typename ServiceType>
                friend class bbox::rt::ServiceReference;

                GenericServiceReference() = delete;
                GenericServiceReference(const GenericServiceReference &) = delete;
                void operator =(const GenericServiceReference &) = delete;

            private:

                GenericServiceReference(const std::string &name, Service &parent, const std::string &referenced_name, TypeInfo type);
                GenericServiceReference(const std::string &name, Resource &parent, const std::string &referenced_name, TypeInfo type);
                ~GenericServiceReference();
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;

                std::string m_ref_name;
                TypeInfo m_type;
                ResourceBase *m_implementing_service;
                void *m_void_ptr;
            };

        } // namespace bbox::rt::details
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__DETAILS__GENERIC_SERVICE_REFERENCE_H__