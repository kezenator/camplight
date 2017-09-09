/**
* @file
*
* Header file for the bbox::debug::DebugProvider class.
*/

#ifndef __BBOX__DEBUG__DEBUG_PROVIDER_H__
#define __BBOX__DEBUG__DEBUG_PROVIDER_H__

#include <memory>
#include <map>
#include <string>

#include <bbox/DebugOutput.h>

namespace bbox
{
    namespace debug
    {
        // Forward declarations
		class DebugQuery;
		class DebugReport;
		class DebugRoot;
        class DebugStream;
		class DebugTarget;
        class DebugVisitor;

        /**
         * A RAII that sets up a thread-local provided for debug output.
         */
        class DebugProvider
        {
            friend class ::bbox::DebugOutput;
			friend class ::bbox::debug::DebugQuery;
			friend class ::bbox::debug::DebugRoot;
			friend class ::bbox::debug::DebugTarget;

            DebugProvider(const DebugProvider &) = delete;
            void operator =(const DebugProvider &) = delete;

        public:
            DebugProvider();
            ~DebugProvider();

        private:

            void RootQuery(DebugVisitor &&visitor);
            void PrintState(DebugOutput &out) const;

            DebugStream *AllocateStream(const char *func, DebugReport *user_report_ptr, std::string &&reason);
            DebugStream *AllocateStream(const char *func, DebugStream *parent_ptr, DebugOutput::E_MIME_TYPE mime_type);
            void DestroyStream(DebugStream *stream);

			void LoginRoot(DebugRoot *root_ptr);
			void LogoutRoot(DebugRoot *root_ptr);

			void LoginTarget(DebugTarget *target_ptr);
			void LogoutTarget(DebugTarget *target_ptr);
            void TargetDebugEnablesUpdated();

            static thread_local DebugProvider *t_provider_ptr;

			struct Pimpl;
			std::unique_ptr<Pimpl> m_pimpl;
        };
    }
}

#endif // __BBOX__DEBUG__DEBUG_PROVIDER_H__
