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

namespace bbox {
    namespace rt { 

        /**
         * A thread pool that can run
         * functions in a separate thread.
         */
        class ThreadPool: public Service
        {
        public:

            static const char *SERVICE_NAME;

            ThreadPool(const std::string &name, Proactor &parent);
            ThreadPool(const std::string &name, Service &parent);
            virtual ~ThreadPool();

        public:
            void HandleStarting() override;
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__THREAD_POOL_H__