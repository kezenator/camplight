/**
 * @file
 *
 * Header for the bbox::rt::ThreadPool class.
 */

#ifndef __BBOX__RT__THREAD_POOL_H__
#define __BBOX__RT__THREAD_POOL_H__

#pragma once

#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/Error.h>

namespace bbox {
    namespace rt {

        // Forward declaration
        class ThreadPoolRef;

        /**
         * A thread pool that can run
         * functions in a separate thread.
         */
        class ThreadPool: public Service
        {
            friend class ThreadPoolRef;

        public:

            static const char *SERVICE_NAME;

            ThreadPool(const std::string &name, Proactor &parent);
            ThreadPool(const std::string &name, Service &parent);
            virtual ~ThreadPool();

        public:

            void Post(
                boost::function<void()> &&function,
                boost::function<void(const bbox::Error &err, const std::string &what)> &&completion_handler);

            void NotifyThreadCompleted();

            void HandleStarting() override;
            void HandleStopping() override;
            void PrintState(bbox::DebugOutput &out) const override;

            struct Pimpl;
            std::unique_ptr<Pimpl> m_pimpl;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__THREAD_POOL_H__
