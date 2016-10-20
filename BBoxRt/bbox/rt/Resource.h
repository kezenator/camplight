/**
 * @file
 *
 * Header for the bbox::rt::Resource class.
 */

#ifndef __BBOX__RT__RESOURCE_H__
#define __BBOX__RT__RESOURCE_H__

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
        class Resource: private details::ResourceBase
        {
            friend class details::ResourceBase;
            friend class details::GenericServiceReference;

        protected:
            Resource(const std::string &name, Service &parent);
            Resource(const std::string &name, Resource &parent);
            ~Resource();

        public:
            using details::ResourceBase::GetProactor;
            using details::ResourceBase::GetOverallRunLevel;
            using details::ResourceBase::GetLocalRunLevel;
			using details::ResourceBase::GetResourceName;
			using details::ResourceBase::GetResourceFullPath;
			using details::ResourceBase::RequestStop;

        protected:
            using details::ResourceBase::HandleStopping;
			using details::ResourceBase::DebugVisit;
			using details::ResourceBase::PrintState;
            using details::ResourceBase::RequestStopAllChildren;
            using details::ResourceBase::NotifyStopped;

        private:
            void HandleStarting() override;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__RESOURCE_H__