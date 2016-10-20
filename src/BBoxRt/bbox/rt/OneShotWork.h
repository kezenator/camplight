/**
 * @file
 *
 * Header for the bbox::rt::OneShotWork class.
 */

#ifndef __BBOX__RT__ONE_SHOT_WORK_H__
#define __BBOX__RT__ONE_SHOT_WORK_H__

#pragma once

#include <bbox/rt/Resource.h>
#include <bbox/rt/Service.h>
#include <boost/function.hpp>

namespace bbox {
    namespace rt { 

        /**
         * A resource that manages a one-shot async work callback.
         * This is a work callback that is posted to be called
         * back asynchronously, and it ensures that at most only one
         * pending call is ever maintained.
         */
        class OneShotWork: private Resource
        {
        public:
            OneShotWork(const std::string &name, Resource &parent, boost::function<void()> &&callback);
            OneShotWork(const std::string &name, Service &parent, boost::function<void()> &&callback);
            ~OneShotWork();

            bool IScheduled() const { return m_pending; }

            void Schedule();

        private:
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;

            void DoCallback();

            bool m_pending;
            boost::function<void()> m_callback;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__ONE_SHOT_WORK_H__