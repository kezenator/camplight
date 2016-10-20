/**
* @file
*
* Implementation for the bbox::rt::Proactor class.
*/

#include <bbox/Assert.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/details/GenericServiceReference.h>

namespace bbox {
    namespace rt {

		Proactor::Proactor()
			: details::ResourceBase(*this)
			, bbox::debug::DebugRoot("resources")
            , m_io_service()
            , m_work_ptr(new boost::asio::io_service::work(m_io_service))
        {
        }

        Proactor::~Proactor()
        {
        }

        Proactor::UnitTestRunner::UnitTestRunner(Proactor &proactor)
            : m_proactor(proactor)
        {
            BBOX_ASSERT(m_proactor.GetOverallRunLevel() == RunLevel::CONSTRUCTED);

            m_proactor.ConnectRegisteredServices();

            m_proactor.RequestStart();

            BBOX_ASSERT(m_proactor.GetOverallRunLevel() == RunLevel::STARTING);
        }

        Proactor::UnitTestRunner::~UnitTestRunner()
        {
            BBOX_ASSERT(m_proactor.GetOverallRunLevel() == RunLevel::STOPPED);
        }

        size_t Proactor::UnitTestRunner::PollOne()
        {
            if (m_proactor.m_io_service.stopped())
                m_proactor.m_io_service.reset();

            return m_proactor.m_io_service.poll_one();
        }

        size_t Proactor::UnitTestRunner::Poll()
        {
            if (m_proactor.m_io_service.stopped())
                m_proactor.m_io_service.reset();

            return m_proactor.m_io_service.poll();
        }

        void Proactor::UnitTestRunner::PollUntilNoneReady()
        {
            while (true)
            {
                size_t done = Poll();
                if (done == 0)
                    return;
            }
        }

        size_t Proactor::UnitTestRunner::RunOne()
        {
            if (m_proactor.m_io_service.stopped())
                m_proactor.m_io_service.reset();

            return m_proactor.m_io_service.run_one();
        }

        void Proactor::UnitTestRunner::Run()
        {
            if (m_proactor.m_io_service.stopped())
                m_proactor.m_io_service.reset();

            m_proactor.m_io_service.run();
        }

        void
        Proactor::Run()
        {
            UnitTestRunner runner(*this);

            runner.Run();
        }

        void Proactor::RequestStop()
        {
            details::ResourceBase::RequestStop([&] () {
                m_work_ptr.reset();
                DisconnectRegisteredServices();
            });
        }

        void Proactor::HandleStarting()
        {
            NotifyStarted();
        }

        void Proactor::HandleStopping()
        {
            NotifyStopped();
            RequestStopAllChildren();
        }

		void Proactor::PrintState(bbox::DebugOutput &out) const
		{
			for (const auto &entry : m_services)
			{
				out.Format("Service Name: %s\n", entry.first);
				out.Format("Service Type: %s\n", entry.second.type.pretty_name());
				
				if (entry.second.implementing_service)
				{
					out.Format("Service Instance: %s\n", entry.second.implementing_service->GetResourceFullPath());
				}
				else
				{
					out.Format("Service Instance: <none>\n");
				}
			}

			for (details::GenericServiceReference *ref_ptr : m_references)
			{
				out.Format("Reference: %s\n", ref_ptr->GetResourceFullPath());
				out.Format("    Name: %s Type: %s\n", ref_ptr->m_ref_name, ref_ptr->m_type.pretty_name());
			}
		}

		void Proactor::VisitDebugRootEntry(bbox::debug::DebugVisitor &&visitor)
		{
			// We just visit ourself (the root node)
			DebugVisit(std::move(visitor));
		}

        void Proactor::ConnectRegisteredServices()
        {
            BBOX_ASSERT(GetOverallRunLevel() == RunLevel::CONSTRUCTED);

            for (details::GenericServiceReference *ref_ptr : m_references)
            {
                auto it = m_services.find(ref_ptr->m_ref_name);
                BBOX_ASSERT(it != m_services.end());

                if (it != m_services.end())
                {
                    const ServiceEntry &entry_ptr = it->second;

                    BBOX_ASSERT(entry_ptr.type == ref_ptr->m_type);

                    if (entry_ptr.type == ref_ptr->m_type)
                    {
                        ref_ptr->m_implementing_service = entry_ptr.implementing_service;
                        ref_ptr->m_void_ptr = entry_ptr.void_ptr;
                    }
                }
            }
        }

        void Proactor::DisconnectRegisteredServices()
        {
            BBOX_ASSERT(GetOverallRunLevel() == RunLevel::STOPPED);

            for (details::GenericServiceReference *ref_ptr : m_references)
            {
                ref_ptr->m_implementing_service = nullptr;
                ref_ptr->m_void_ptr = nullptr;
            }
        }

    } // namespace bbox::rt
} // namespace bbox
