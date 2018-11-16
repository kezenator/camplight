/**
 * @file
 *
 * Header for the bbox::rt::ServiceReference class.
 */

#ifndef __BBOX__RT__SERVICE_REFERENCE_H__
#define __BBOX__RT__SERVICE_REFERENCE_H__

#pragma once

#include <bbox/rt/details/GenericServiceReference.h>
#include <bbox/Assert.h>

namespace bbox {
    namespace rt {

        /**
         * Reference to a service.
         */
        template <typename ServiceType>
        class ServiceReference : public details::GenericServiceReference
        {
        public:
            ServiceReference(const std::string &name, Service &parent, const std::string &referenced_name)
                : details::GenericServiceReference(name, parent, referenced_name, typeid(ServiceType))
            {
            }

            ~ServiceReference()
            {
            }

            bool IsReferenceConnected() const
            {
                return m_void_ptr != nullptr;
            }

            const ServiceType *operator ->() const
            {
                BBOX_ASSERT(m_void_ptr != nullptr);
                return static_cast<const ServiceType *>(m_void_ptr);
            }

            ServiceType *operator ->()
            {
                BBOX_ASSERT(m_void_ptr != nullptr);
                return static_cast<ServiceType *>(m_void_ptr);
            }

            const ServiceType &operator *() const
            {   
                BBOX_ASSERT(m_void_ptr != nullptr);
                return *static_cast<const ServiceType *>(m_void_ptr);
            }

            ServiceType &operator *()
            {
                BBOX_ASSERT(m_void_ptr != nullptr);
                return *static_cast<ServiceType *>(m_void_ptr);
            }
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__SERVICE_REFERENCE_H__