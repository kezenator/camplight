/**
 * @file
 *
 * Header for the bbox::rt::ResourceBase class.
 */

#ifndef __BBOX__RT__DETAILS__RESOURCE_BASE_H__
#define __BBOX__RT__DETAILS__RESOURCE_BASE_H__

#pragma once

#include <map>
#include <set>

#include <boost/function.hpp>

#include <bbox/rt/RunLevel.h>
#include <bbox/TypeInfo.h>
#include <bbox/DebugOutput.h>
#include <bbox/debug/DebugVisitor.h>

namespace bbox {
    namespace rt { 

        // Forward declarations
        class Proactor;
        class Service;
        class Resource;

        namespace details {

            // Forward declarations
            class GenericServiceReference;

            /**
             * Base class for all objects in the resource hierarchy.
             */
            class ResourceBase
            {
                ResourceBase() = delete;
                ResourceBase(const ResourceBase &) = delete;
                ResourceBase(ResourceBase &&) = delete;
                void operator =(const ResourceBase &) = delete;
                void operator =(ResourceBase &&) = delete;

                friend class ::bbox::rt::Proactor;
                friend class ::bbox::rt::Service;
                friend class ::bbox::rt::Resource;
                friend class ::bbox::rt::details::GenericServiceReference;

            private:
                ResourceBase(Proactor &proactor);
                ResourceBase(const std::string &name, ResourceBase &parent, RunLevel initial_level);
                virtual ~ResourceBase();

                Proactor &GetProactor() { return m_proactor; }

                /** 
                 * Gets the overall run level - this represents the run level
                 * of this and all sub-items.
                 *
                 * The value of this controls the "Request*" actions.
                 * If constructed, RequestStart can be called.
                 * If running, RequestStop can be called.
                 */
                RunLevel GetOverallRunLevel() const { return m_overall_level; }

                /**
                 * Gets the local run level of this item.
                 *
                 * This value controls the "Notify*" actions.
                 * If Starting, NotifyStarted can be called.
                 * If Stopping, NotifyStop can be called./
                 */
                RunLevel GetLocalRunLevel() const { return m_local_level; }

				const std::string &GetResourceName() const { return m_name; }
				const std::string &GetResourceFullPath() const { return m_full_path; }
                void PrintResourcePathLink(bbox::DebugOutput &out) const;

                virtual void HandleStarting() = 0;
                virtual void HandleStopping() = 0;
				virtual void PrintState(bbox::DebugOutput &out) const = 0;
				virtual void DebugVisit(bbox::debug::DebugVisitor &&visitor);

                void RequestStart(boost::function<void()> completion_handler = boost::function<void()>());
                void RequestStop(boost::function<void()> completion_handler = boost::function<void()>());
                void RequestStopAllChildren();
                void NotifyStarted();
                void NotifyStopped();

                void GenericRegisterService(const std::string &name, TypeInfo type, void *service);
                void SetThisDependantOn(::bbox::rt::Service &service);
                void SetThisDependantOn(::bbox::rt::details::GenericServiceReference &service_ref);

                void PostUpdateWork();
                void HandleUpdateWork();

                class MethodEntry;
                void UpdateLevel(RunLevel &level, RunLevel new_value);


                ResourceBase *m_parent_ptr;
                Proactor &m_proactor;
                const std::string m_name;
                const std::string m_full_path;
                RunLevel m_local_level;
                RunLevel m_overall_level;
                std::map<std::string, ResourceBase *> m_children;
                bool m_work_requested;
                boost::function<void()> m_request_callback;
                std::set<GenericServiceReference *> m_references_from;

                std::set<ResourceBase *> m_dependant_on_them;
                std::set<GenericServiceReference *> m_dependant_on_their_ref;
                std::set<ResourceBase *> m_dependant_on_us;
            };

        } // namespace bbox::rt::details
    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__DETAILS__RESOURCE_BASE_H__