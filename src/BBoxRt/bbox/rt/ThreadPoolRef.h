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

            void RunOnThread(boost::function<void()> &&function,
                             boost::function<void(const bbox::Error &err, const std::string &what)> &&completion_handler);

            bool AnyOperationsRunning() const { return m_items_in_progress != 0; }

        private:
            void HandleStarting() override;
            void HandleStopping() override;
            void PrintState(bbox::DebugOutput &out) const override;

            void CheckStopped();

            size_t m_items_in_progress;
            size_t m_items_completed;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__THREAD_POOL_REF_H__
