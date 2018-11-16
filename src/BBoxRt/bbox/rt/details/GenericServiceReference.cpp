/**
* @file
*
* Implementation for the bbox::rt::details::GenericServiceReference class.
*/

#include <bbox/rt/details/GenericServiceReference.h>
#include <bbox/rt/Proactor.h>
#include <bbox/Assert.h>

namespace bbox {
    namespace rt {
        namespace details {

            GenericServiceReference::GenericServiceReference(const std::string &name, Service &parent, const std::string &referenced_name, TypeInfo type)
                : Service(name, parent)
                , m_ref_name(referenced_name)
                , m_type(type)
                , m_implementing_service(nullptr)
                , m_void_ptr(nullptr)
            {
                BBOX_ASSERT(GetProactor().GetOverallRunLevel() == RunLevel::CONSTRUCTED);

                auto insert_result_1 = GetProactor().m_references.insert(this);
                BBOX_ASSERT(insert_result_1.second);

                auto insert_result_2 = m_parent_ptr->m_references_from.insert(this);
                BBOX_ASSERT(insert_result_2.second);
            }

            GenericServiceReference::~GenericServiceReference()
            {
                // Ensure we've already been disconnected
                BBOX_ASSERT(m_implementing_service == nullptr);
                BBOX_ASSERT(m_void_ptr == nullptr);
            }

            void GenericServiceReference::HandleStarting()
            {
                NotifyStarted();
            }

            void GenericServiceReference::HandleStopping()
            {
                NotifyStopped();
                RequestStopAllChildren();
            }

			void GenericServiceReference::PrintState(bbox::DebugOutput &out) const
			{
				out.Format("Referenced service name: %s\n", m_ref_name);
				out.Format("Referenced service type: %s\n", m_type.pretty_name());
                
                out << "Referenced service inst: ";
                if (m_implementing_service)
                    m_implementing_service->PrintResourcePathLink(out);
                else
                    out << "<none>";
                out << std::endl;
			}

        } // namespace bbox::rt::details
    } // namespace bbox::rt
} // namespace bbox
