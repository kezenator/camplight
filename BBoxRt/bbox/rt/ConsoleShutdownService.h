/**
 * @file
 *
 * Header for the bbox::rt::ConsoleShutdownService class.
 */

#ifndef __BBOX__RT__CONSOLE_SHUTDOWN_SERVICE_H__
#define __BBOX__RT__CONSOLE_SHUTDOWN_SERVICE_H__

#pragma once

#include <bbox/rt/Service.h>

namespace bbox {
    namespace rt { 

        /**
         * A service that shuts down the process
         * using the win32 console CTRL+C handler.
         */
        class ConsoleShutdownService : public Service
        {
        public:
            ConsoleShutdownService(const std::string &name, Proactor &parent);
            ConsoleShutdownService(const std::string &name, Service &parent);
            virtual ~ConsoleShutdownService();

        private:
            void HandleStarting() override;
            void HandleStopping() override;
			void PrintState(bbox::DebugOutput &out) const override;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__CONSOLE_SHUTDOWN_SERVICE_H__