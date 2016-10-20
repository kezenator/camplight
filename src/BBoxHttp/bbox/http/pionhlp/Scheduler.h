/**
 * @file
 *
 * Header for the bbox::http::pionhlp::Scheduler class.
 */

#ifndef __BBOX__RT__PIONHLP__SCHEDULER_H__
#define __BBOX__RT__PIONHLP__SCHEDULER_H__

#pragma once

#include <pion/scheduler.hpp>

#include <bbox/rt/Resource.h>

namespace bbox {
    namespace http {
        namespace pionhlp {

            class Scheduler : public rt::Resource, public pion::scheduler
            {
            public:
                Scheduler(const std::string &name, rt::Service &parent);
                Scheduler(const std::string &name, rt::Resource &parent);
                ~Scheduler();

                // Implements pion::scheduler
                boost::asio::io_service &get_io_service() override;


            private:
                // Implements bbox::rt::Resource
                void HandleStopping() override;
				void PrintState(bbox::DebugOutput &out) const override;
            };

        } // namespace bbox::http::pionhlp
    } // namespace bbox::http
} // namespace bbox

#endif // __BBOX__RT__PIONHLP__SCHEDULER_H__