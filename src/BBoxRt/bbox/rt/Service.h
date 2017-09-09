/**
 * @file
 *
 * Header for the bbox::rt::Service class.
 */

#ifndef __BBOX__RT__SERVICE_H__
#define __BBOX__RT__SERVICE_H__

#pragma once

#include <bbox/rt/details/ResourceBase.h>
#include <bbox/rt/Proactor.h>


namespace bbox {
    namespace rt { 

        // Forward declaration
        namespace details {
            class GenericServiceReference;
        }

        /**
         * Top level proactor that
         * runs an application.
         */
        class Service: private details::ResourceBase
        {
            friend class Resource;
            friend class details::ResourceBase;
            friend class details::GenericServiceReference;

        public:
            Service(const std::string &name, Proactor &parent);
            Service(const std::string &name, Service &parent);
            virtual ~Service();

        public:
            using details::ResourceBase::GetProactor;
            using details::ResourceBase::GetOverallRunLevel;
            using details::ResourceBase::GetLocalRunLevel;
			using details::ResourceBase::GetResourceName;
			using details::ResourceBase::GetResourceFullPath;
            using details::ResourceBase::PrintResourcePathLink;
			using details::ResourceBase::RequestStart;
            using details::ResourceBase::RequestStop;
            using details::ResourceBase::SetThisDependantOn;

        protected:
            using details::ResourceBase::HandleStarting;
            using details::ResourceBase::HandleStopping;
			using details::ResourceBase::PrintState;
			using details::ResourceBase::DebugVisit;
			using details::ResourceBase::RequestStopAllChildren;
            using details::ResourceBase::NotifyStarted;
            using details::ResourceBase::NotifyStopped;

            template <typename ServiceType>
            void RegisterService(const std::string &name, ServiceType *service)
            {
                GenericRegisterService(name, typeid(ServiceType), service);
            }
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__SERVICE_H__