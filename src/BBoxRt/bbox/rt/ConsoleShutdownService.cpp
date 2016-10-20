/**
 * @file
 *
 * Implementation for the bbox::rt::ConsoleShutdownService class.
 */

#include <bbox/rt/ConsoleShutdownService.h>
#include <bbox/rt/Proactor.h>
#include <bbox/Assert.h>

#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#ifdef WIN32
#include <Windows.h>
#else
#include <signal.h>
#endif // not WIN32

namespace {

    void RunLocked(const boost::function<void()> &func)
    {
        static boost::mutex g_mutex;

        boost::unique_lock<boost::mutex> lock(g_mutex);

        func();
    }

    bbox::rt::ConsoleShutdownService *&ServicePointer()
    {
        static bbox::rt::ConsoleShutdownService *g_pointer = 0;

        return g_pointer;
    }

#ifdef WIN32

    void SameThreadShutdownHandler()
    {
        RunLocked([]()
        {
            bbox::rt::ConsoleShutdownService *service_ptr = ServicePointer();

            if (service_ptr
                && (service_ptr->GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING))
            {
                service_ptr->GetProactor().RequestStop();
            }
        });
    }
    
    BOOL WINAPI ShutdownHandler(DWORD ctrl_type)
    {
        RunLocked([] () {
            bbox::rt::ConsoleShutdownService *service_ptr = ServicePointer();
            
            if (service_ptr)
                service_ptr->GetProactor().Post(&SameThreadShutdownHandler);
        });
        return TRUE;
    }
    
    void AddHandler()
    {
        SetConsoleCtrlHandler(&ShutdownHandler, TRUE);
    }
    
    void RemoveHandler()
    {
        SetConsoleCtrlHandler(&ShutdownHandler, FALSE);
    }

#else
    
    static void ctrl_c_handler(int signum)
    {
        RunLocked([]()
        {
            bbox::rt::ConsoleShutdownService *service_ptr = ServicePointer();

            if (service_ptr
                && (service_ptr->GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING))
            {
                service_ptr->GetProactor().RequestStop();
            }
        });
    }

    static void AddHandler(void)
    {
        struct sigaction sa;

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = ctrl_c_handler;

        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGHUP, &sa, NULL);
    }
    
    void RemoveHandler()
    {
        struct sigaction sa;

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_DFL;

        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGHUP, &sa, NULL);
    }

#endif // not WIN32

} // annon namespace

namespace bbox {
    namespace rt {

        ConsoleShutdownService::ConsoleShutdownService(const std::string &name, Proactor &parent)
            : Service(name, parent)
        {
            RunLocked([&] () {
                BBOX_ASSERT(ServicePointer() == nullptr);
                ServicePointer() = this;
                    
                AddHandler();
            });
        }

        ConsoleShutdownService::ConsoleShutdownService(const std::string &name, Service &parent)
            : Service(name, parent)
        {
            RunLocked([&]()
            {
                BBOX_ASSERT(ServicePointer() == nullptr);
                ServicePointer() = this;
                    
                AddHandler();
            });
        }

        ConsoleShutdownService::~ConsoleShutdownService()
        {
            RunLocked([&]()
            {
                BBOX_ASSERT(ServicePointer() == this);
                ServicePointer() = nullptr;
                    
                RemoveHandler();
            });
        }

        void ConsoleShutdownService::HandleStarting()
        {
            NotifyStarted();
        }

        void ConsoleShutdownService::HandleStopping()
        {
            NotifyStopped();
            RequestStopAllChildren();
        }

		void ConsoleShutdownService::PrintState(bbox::DebugOutput &out) const
		{
		}

    } // namespace bbox::rt
} // namespace bbox
