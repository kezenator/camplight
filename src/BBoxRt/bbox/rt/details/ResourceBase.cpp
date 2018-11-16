/**
* @file
*
* Implementation for the bbox::rt::details::ResourceBase class.
*/

#include <iostream>

#include <boost/bind.hpp>

#include <bbox/Assert.h>
#include <bbox/Format.h>

#include <bbox/DebugOutput.h>
#include <bbox/debug/DebugReport.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/rt/details/ResourceBase.h>
#include <bbox/rt/details/GenericServiceReference.h>

namespace bbox {
    namespace rt {
        namespace details {

            class ResourceBase::MethodEntry
            {
            public:
                MethodEntry(ResourceBase &resource, const char *method)
                {
                    // TODO - conditional enable
                    if (false)
                    {
                        bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Testing);
                        out << resource.m_full_path
                            << " : "
                            << method
                            << " ["
                            << resource.m_overall_level.ToString()
                            << ", "
                            << resource.m_local_level.ToString()
                            << "]"
                            << std::endl;
                    }
                }

                ~MethodEntry()
                {
                }
            };

            ResourceBase::ResourceBase(Proactor &proactor)
                : m_parent_ptr(nullptr)
                , m_proactor(proactor)
                , m_name("resources")
                , m_full_path("/resources")
                , m_local_level(RunLevel::CONSTRUCTED)
                , m_overall_level(RunLevel::CONSTRUCTED)
                , m_children()
                , m_work_requested(false)
                , m_request_callback()
                , m_references_from()
                , m_dependant_on_them()
                , m_dependant_on_us()
            {
                MethodEntry entry(*this, "Constructor");
            }

            ResourceBase::ResourceBase(const std::string &name, ResourceBase &parent, RunLevel initial_level)
                : m_parent_ptr(&parent)
                , m_proactor(parent.m_proactor)
                , m_name(name)
                , m_full_path(Format("%s/%s", parent.m_full_path, name))
                , m_local_level(initial_level)
                , m_overall_level(initial_level)
                , m_children()
                , m_work_requested(false)
                , m_request_callback()
            {
                MethodEntry entry(*this, "Constructor");

                BBOX_ASSERT(&parent != this);
                BBOX_ASSERT((m_local_level == RunLevel::CONSTRUCTED) || (m_local_level == RunLevel::RUNNING));
                if (m_local_level == RunLevel::CONSTRUCTED)
                {
                    // Can only create a constructed child (i.e. Service) if the parent is also constructed
                    BBOX_ASSERT(parent.m_overall_level == RunLevel::CONSTRUCTED);
                }

                auto insert_result = parent.m_children.insert(std::make_pair(name, this));

                // Can't have duplicate services
                BBOX_ASSERT(insert_result.second);
            }

            ResourceBase::~ResourceBase()
            {
                MethodEntry entry(*this, "Destructor");

                BBOX_ASSERT(m_overall_level == RunLevel::STOPPED);
                BBOX_ASSERT(m_children.empty());
                BBOX_ASSERT(m_work_requested == false);

                if (m_parent_ptr)
                {
                    auto it = m_parent_ptr->m_children.find(m_name);

                    BBOX_ASSERT(it != m_parent_ptr->m_children.end());

                    m_parent_ptr->m_children.erase(it);
                }

                while (!m_dependant_on_them.empty())
                {
                    auto our_it = m_dependant_on_them.begin();
                    ResourceBase *other = *our_it;
                    m_dependant_on_them.erase(our_it);

                    auto other_it = other->m_dependant_on_us.find(this);
                    BBOX_ASSERT(other_it != other->m_dependant_on_us.end());
                    other->m_dependant_on_us.erase(other_it);
                }

                while (!m_dependant_on_us.empty())
                {
                    auto our_it = m_dependant_on_us.begin();
                    ResourceBase *other = *our_it;
                    m_dependant_on_us.erase(our_it);

                    auto other_it = other->m_dependant_on_them.find(this);
                    BBOX_ASSERT(other_it != other->m_dependant_on_them.end());
                    other->m_dependant_on_them.erase(other_it);
                }
            }

            void ResourceBase::PrintResourcePathLink(bbox::DebugOutput &out) const
            {
                bbox::DebugOutput link_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Debug_Full_Path);
                if (link_out)
                {
                    link_out << m_full_path;
                }
            }

			void ResourceBase::DebugVisit(bbox::debug::DebugVisitor &&visitor)
			{
				if (visitor.WantVisitChildren())
				{
					for (auto &entry : m_children)
					{
						entry.second->DebugVisit(visitor.ChildVisitor(entry.first));
					}
				}

                if (visitor.WantEnumerateChildren())
                {
                    for (auto &entry : m_children)
                    {
                        visitor.EnumerateChild(entry.first, bbox::TypeInfo(typeid(*entry.second)).pretty_name());
                    }
                }

                if (visitor.WantReport())
				{
					debug::DebugReport report;

					{
						DebugOutput out(
							BBOX_FUNC,
							report,
							bbox::Format("Resource %s", m_full_path));

						out.Format("Resource Name: %s\n", m_name);
                        out.Format("Resource Path: ");
                        {
                            DebugOutput path_out(BBOX_FUNC, out, DebugOutput::Mime_Text_Debug_Full_Path);
                            path_out << m_full_path;
                        }
                        out << std::endl;
						out.Format("Resource Type: %s\n", TypeInfo(typeid(*this)).pretty_name());
						out.Format("\n");

						PrintState(out);
					}

					visitor.SetReport(report);
				}
			}

            void ResourceBase::RequestStart(boost::function<void()> completion_handler)
            {
                MethodEntry entry(*this, "RequestStart");

                BBOX_ASSERT(m_overall_level == RunLevel::CONSTRUCTED);

                m_request_callback = std::move(completion_handler);
                UpdateLevel(m_overall_level, RunLevel::STARTING);
                PostUpdateWork();
            }

            void ResourceBase::RequestStop(boost::function<void()> completion_handler)
            {
                MethodEntry entry(*this, "RequestStop");

                BBOX_ASSERT(m_overall_level == RunLevel::RUNNING);

                m_request_callback = std::move(completion_handler);
                UpdateLevel(m_overall_level, RunLevel::STOPPING);
                PostUpdateWork();
            }

            void ResourceBase::RequestStopAllChildren()
            {
                MethodEntry entry(*this, "RequestStopAllChildren");

                for (auto it = m_children.begin();
                     it != m_children.end();
                     ++it)
                {
                    ResourceBase *child = it->second;

                    BBOX_ASSERT((child->m_overall_level == RunLevel::RUNNING)
                                || (child->m_overall_level == RunLevel::STOPPING)
                                || (child->m_overall_level == RunLevel::STOPPED));

                    if (child->m_overall_level == RunLevel::RUNNING)
                    {
                        child->RequestStop();
                    }
                }
            }

            void ResourceBase::NotifyStarted()
            {
                MethodEntry entry(*this, "NotifyStarted");

                BBOX_ASSERT(m_local_level == RunLevel::STARTING);

                UpdateLevel(m_local_level, RunLevel::RUNNING);
                PostUpdateWork();
            }

            void ResourceBase::NotifyStopped()
            {
                MethodEntry entry(*this, "NotifyStopped");

                BBOX_ASSERT(m_local_level == RunLevel::STOPPING);

                UpdateLevel(m_local_level, RunLevel::STOPPED);
                PostUpdateWork();
            }

            void ResourceBase::GenericRegisterService(const std::string &name, TypeInfo type, void *service)
            {
                // Ensure there isn't already a service with this name

                auto existing_it = m_proactor.m_services.find(name);
                BBOX_ASSERT(existing_it == m_proactor.m_services.end());

                if (existing_it == m_proactor.m_services.end())
                {
                    m_proactor.m_services[name].type = type;
                    m_proactor.m_services[name].implementing_service = this;
                    m_proactor.m_services[name].void_ptr = service;
                }
            }

            void ResourceBase::SetThisDependantOn(::bbox::rt::Service &service)
            {
                BBOX_ASSERT(m_local_level == RunLevel::CONSTRUCTED);
                BBOX_ASSERT(m_overall_level == RunLevel::CONSTRUCTED);

                BBOX_ASSERT(service.m_local_level == RunLevel::CONSTRUCTED);
                BBOX_ASSERT(service.m_overall_level == RunLevel::CONSTRUCTED);

                auto insert_result_1 = m_dependant_on_them.insert(&service);
                auto insert_result_2 = service.m_dependant_on_us.insert(this);

                BBOX_ASSERT(insert_result_1.second);
                BBOX_ASSERT(insert_result_2.second);
            }

            void ResourceBase::SetThisDependantOn(::bbox::rt::details::GenericServiceReference &service_ref)
            {
                BBOX_ASSERT(m_local_level == RunLevel::CONSTRUCTED);
                BBOX_ASSERT(m_overall_level == RunLevel::CONSTRUCTED);

                BBOX_ASSERT(service_ref.m_local_level == RunLevel::CONSTRUCTED);
                BBOX_ASSERT(service_ref.m_overall_level == RunLevel::CONSTRUCTED);
                BBOX_ASSERT(!service_ref.m_implementing_service);

                auto insert_result_1 = m_dependant_on_their_ref.insert(&service_ref);
                auto insert_result_2 = service_ref.m_dependant_on_our_service.insert(this);

                BBOX_ASSERT(insert_result_1.second);
                BBOX_ASSERT(insert_result_2.second);
            }

            void ResourceBase::PostUpdateWork()
            {
                if (!m_work_requested)
                {
                    m_work_requested = true;

                    m_proactor.m_io_service.post(boost::bind(
                        &ResourceBase::HandleUpdateWork,
                        this));
                }
            }

            void ResourceBase::HandleUpdateWork()
            {
                MethodEntry entry(*this, "HandleUpdateWork");

                BBOX_ASSERT(m_work_requested);
                m_work_requested = false;

                if (m_overall_level == m_local_level)
                {
                    // We're in a stable state - this work was
                    // triggered but not needed.
                }
                else if ((m_overall_level == RunLevel::STARTING)
                         && (m_local_level == RunLevel::CONSTRUCTED))
                {
                    // We need to ensure all dependant services are
                    // started. Once that's done, then we're able
                    // to locall start this service

                    bool all_dependants_running = true;

                    for (auto it = m_children.begin();
                         it != m_children.end();
                         ++it)
                    {
                        ResourceBase *child = it->second;

                        if (child->m_overall_level != RunLevel::RUNNING)
                        {
                            all_dependants_running = false;
                        }

                        if (child->m_overall_level == RunLevel::CONSTRUCTED)
                        {
                            child->RequestStart();
                        }
                    }

                    for (GenericServiceReference *ref_ptr : m_references_from)
                    {
                        auto it = GetProactor().m_services.find(ref_ptr->m_ref_name);
                        if (it == GetProactor().m_services.end())
                        {
                            // This service doesn't exist
                            BBOX_ASSERT(false);
                        }
                        else
                        {
                            const Proactor::ServiceEntry &entry = it->second;

                            if (entry.implementing_service->m_overall_level != RunLevel::RUNNING)
                            {
                                all_dependants_running = false;
                            }
                        }
                    }

                    for (ResourceBase *dep_ptr : m_dependant_on_them)
                    {
                        if (dep_ptr->m_overall_level != RunLevel::RUNNING)
                        {
                            all_dependants_running = false;
                        }
                    }

                    if (all_dependants_running)
                    {
                        UpdateLevel(m_local_level, RunLevel::STARTING);

                        MethodEntry entry(*this, "HandleStarting");

                        HandleStarting();
                    }
                }
                else if ((m_overall_level == RunLevel::STARTING)
                         && (m_local_level == RunLevel::RUNNING))
                {
                    // Notify start has been called. Just mark ourselves
                    // as overall running and get our parent and any dependant
                    // service references to update themselves.

                    UpdateLevel(m_overall_level, RunLevel::RUNNING);

                    if (m_parent_ptr)
                    {
                        m_parent_ptr->PostUpdateWork();
                    }

                    for (details::GenericServiceReference *ref_ptr : GetProactor().m_references)
                    {
                        if (ref_ptr->m_implementing_service == this)
                        {
                            BBOX_ASSERT(ref_ptr->m_parent_ptr);

                            ref_ptr->m_parent_ptr->PostUpdateWork();
                        }
                    }

                    for (ResourceBase *dep_ptr : m_dependant_on_us)
                    {
                        if (dep_ptr->m_overall_level == RunLevel::STARTING)
                        {
                            dep_ptr->PostUpdateWork();
                        }
                    }

                    boost::function<void()> callback = std::move(m_request_callback);

                    if (callback)
                    {
                        callback();
                    }
                }
                else if ((m_overall_level == RunLevel::STOPPING)
                         && (m_local_level == RunLevel::RUNNING))
                {
                    // RequestStop has been called - if all dependant
                    // services that have refernces to us, then
                    // we can notify it

                    bool all_dependants_stopped = true;

                    for (details::GenericServiceReference *ref_ptr : GetProactor().m_references)
                    {
                        if (ref_ptr->m_implementing_service == this)
                        {
                            BBOX_ASSERT(ref_ptr->m_parent_ptr);

                            if (ref_ptr->m_parent_ptr->GetOverallRunLevel() != RunLevel::STOPPED)
                            {
                                all_dependants_stopped = false;
                            }
                        }
                    }

                    for (ResourceBase *dep_ptr : m_dependant_on_us)
                    {
                        if (dep_ptr->m_overall_level != RunLevel::STOPPED)
                        {
                            all_dependants_stopped = false;
                        }
                    }

                    if (all_dependants_stopped)
                    {
                        UpdateLevel(m_local_level, RunLevel::STOPPING);

                        MethodEntry entry(*this, "HandleStopping");

                        HandleStopping();
                    }
                }
                else if ((m_overall_level == RunLevel::STOPPING)
                         && (m_local_level == RunLevel::STOPPED))
                {
                    // Once all children are stopped we update
                    // the overall state to stopped

                    bool all_children_stopped = true;

                    for (auto it = m_children.begin();
                         it != m_children.end();
                         ++it)
                    {
                        ResourceBase *child = it->second;

                        if (child->m_overall_level != RunLevel::STOPPED)
                        {
                            all_children_stopped = false;
                        }
                    }

                    if (all_children_stopped)
                    {
                        // All the children have stopped and we have stopped -
                        // we can update our overall level to stopped.
                        // Also, we need to check the parent and also
                        // and dependant services that we're referencing.
                        // Finally - we call the callback notification.

                        UpdateLevel(m_overall_level, RunLevel::STOPPED);

                        if (m_parent_ptr)
                        {
                            m_parent_ptr->PostUpdateWork();
                        }

                        for (GenericServiceReference *ref_ptr : m_references_from)
                        {
                            if (ref_ptr->m_implementing_service)
                            {
                                ref_ptr->m_implementing_service->PostUpdateWork();
                            }
                        }

                        for (ResourceBase *dep_ptr : m_dependant_on_them)
                        {
                            if (dep_ptr->m_overall_level == RunLevel::STOPPING)
                            {
                                dep_ptr->PostUpdateWork();
                            }
                        }

                        boost::function<void()> callback = std::move(m_request_callback);

                        if (callback)
                        {
                            callback();
                        }
                    }
                }
                else
                {
                    // Unsupported transition
                    BBOX_ASSERT(false);
                }
            }

            void ResourceBase::UpdateLevel(RunLevel &level, RunLevel new_value)
            {
                BBOX_ASSERT((&level == &m_overall_level)
                            || (&level == &m_local_level));

                // TODO - conditional enable
                //if ((this == &m_proactor) && (&level == &m_overall_level))
                {
                    bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Activity);
                    out << m_full_path
                        << " : Update "
                        << ((&level == &m_overall_level)
                            ? "Overall"
                            : "Local")
                        << " Level : "
                        << level.ToString()
                        << " => "
                        << new_value.ToString()
                        << std::endl;
                }

                level = new_value;
            }

        } // namespace bbox::rt::details
    } // namespace bbox::rt
} // namespace bbox
