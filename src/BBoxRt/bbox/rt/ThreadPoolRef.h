/**
 * @file
 *
 * Header for the bbox::rt::ThreadPoolRef class.
 */

#ifndef __BBOX__RT__THREAD_POOL_REF_H__
#define __BBOX__RT__THREAD_POOL_REF_H__

#pragma once

#include <bbox/rt/ServiceReference.h>
#include <bbox/rt/ThreadPool.h>
#include <bbox/Error.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace bbox {
    namespace rt {

        /**
         * Reference to the thread pool.
         */
        class ThreadPoolRef : public ServiceReference<ThreadPool>
        {
        public:
            ThreadPoolRef(const std::string &name, Service &parent);
            ~ThreadPoolRef();

            void RunOnThread(boost::function<void()> function,
                             boost::function<void(const Error &err, const std::string &what)> completion_handler);

            bool AnyOperationsRunning() const { return m_pending_ops != 0; }

        private:
            class Operation;
            class Functor;

            void HandleStarting() override;
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;

            void HandleComplete(std::shared_ptr<Operation> operation_ptr, const Error &error, const std::string &what);
            void CheckStopped();

            size_t m_pending_ops;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__THREAD_POOL_REF_H__