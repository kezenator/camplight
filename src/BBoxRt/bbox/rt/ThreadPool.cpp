/**
* @file
*
* Implementation for the bbox::rt::ThreadPool class.
*/

#include <bbox/rt/ThreadPool.h>

namespace bbox {
    namespace rt {

        const char *ThreadPool::SERVICE_NAME = "bbox::rt::ThreadPool";

        ThreadPool::ThreadPool(const std::string &name, Proactor &parent)
            : Service(name, parent)
        {
            RegisterService(SERVICE_NAME, this);
        }

        ThreadPool::ThreadPool(const std::string &name, Service &parent)
            : Service(name, parent)
        {
            RegisterService(SERVICE_NAME, this);
        }

        ThreadPool::~ThreadPool()
        {
        }

        void ThreadPool::HandleStarting()
        {
            NotifyStarted();
        }

        void ThreadPool::HandleStopping()
        {
            NotifyStopped();
            RequestStopAllChildren();
        }

		void ThreadPool::PrintState(bbox::DebugOutput & /*out*/) const
		{
		}

    } // namespace bbox::rt
} // namespace bbox
