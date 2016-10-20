/**
 * @file
 *
 * Header for the bbox::rt::Proactor class.
 */

#ifndef __BBOX__RT__PROACTOR_H__
#define __BBOX__RT__PROACTOR_H__

#pragma once

#include <map>
#include <memory>
#include <set>

#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>

#include <bbox/debug/DebugRoot.h>
#include <bbox/rt/details/ResourceBase.h>

namespace bbox {
    namespace rt { 

        // Forward declarations
        namespace details {
            class GenericServiceReference;
        }

        /**
         * Top level proactor that
         * runs an application.
         */
        class Proactor: private details::ResourceBase,
			            private bbox::debug::DebugRoot
        {
            friend class details::GenericServiceReference;
            friend class details::ResourceBase;
            friend class Service;

        public:

            class UnitTestRunner
            {
                UnitTestRunner() = delete;
                UnitTestRunner(const UnitTestRunner &) = delete;
                void operator =(const UnitTestRunner &) = delete;

            public:
                UnitTestRunner(Proactor &proactor);
                ~UnitTestRunner();

                size_t PollOne();
                size_t Poll();
                void PollUntilNoneReady();
                size_t RunOne();
                void Run();

            private:
                Proactor &m_proactor;
            };

            Proactor();
            ~Proactor();

            using details::ResourceBase::GetOverallRunLevel;

            boost::asio::io_service &GetIoService()
            {
                return m_io_service;
            }

            void Run();
            void RequestStop();

            void Post(boost::function<void()> &&func)
            {
                m_io_service.post(std::move(func));
            }

            void ThreadSafePost(boost::function<void()> &&func)
            {
                m_io_service.post(std::move(func));
            }

        private:

            void HandleStarting() override;
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;

			void VisitDebugRootEntry(bbox::debug::DebugVisitor &&visitor) override;

            void ConnectRegisteredServices();
            void DisconnectRegisteredServices();

            struct ServiceEntry
            {
                TypeInfo type;
                details::ResourceBase *implementing_service;
                void *void_ptr;
            };

            boost::asio::io_service m_io_service;
            std::unique_ptr<boost::asio::io_service::work> m_work_ptr;
            std::map<std::string, ServiceEntry> m_services;
            std::set<details::GenericServiceReference *> m_references;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__PROACTOR_H__