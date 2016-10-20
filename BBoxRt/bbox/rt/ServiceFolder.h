/**
 * @file
 *
 * Header for the bbox::rt::ServiceFolder class.
 */

#ifndef __BBOX__RT__SERVICE_FOLDER_H__
#define __BBOX__RT__SERVICE_FOLDER_H__

#pragma once

#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>

namespace bbox {
    namespace rt { 

        /**
         * A folder for grouping multiple child services/resources.
         */
        class ServiceFolder : public Service
        {
        public:

            ServiceFolder(const std::string &name, Proactor &parent);
            ServiceFolder(const std::string &name, Service &parent);
            virtual ~ServiceFolder();

        public:
            void HandleStarting() override;
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__SERVICE_FOLDER_H__