/**
* @file
*
* Implementation for the bbox::rt::ThreadPoolRef class.
*/

#include <bbox/rt/ThreadPoolRef.h>
#include <bbox/Assert.h>
#include <bbox/Exception.h>

#include <boost/thread.hpp>

namespace bbox {
    namespace rt {

        ThreadPoolRef::ThreadPoolRef(const std::string &name, Service &parent)
            : ServiceReference<ThreadPool>(name, parent, ThreadPool::SERVICE_NAME)
            , m_pending_ops(0)
        {
        }

        ThreadPoolRef::~ThreadPoolRef()
        {
            BBOX_ASSERT(m_pending_ops == 0);
        }
        
        void ThreadPoolRef::HandleStarting()
        {
            NotifyStarted();
        }

        void ThreadPoolRef::HandleStopping()
        {
            CheckStopped();
        }

		void ThreadPoolRef::PrintState(bbox::DebugOutput &out) const
		{
			out.Format("Pending operations: %d\n", m_pending_ops);
		}

        void ThreadPoolRef::CheckStopped()
        {
            if ((GetLocalRunLevel() == RunLevel::STOPPING)
                && (m_pending_ops == 0))
            {
                NotifyStopped();
                RequestStopAllChildren();
            }
        }

        class ThreadPoolRef::Operation
        {
        public:
            ThreadPoolRef &owner;
            boost::function<void()> function;
            boost::function<void(const Error &err, const std::string &what)> completion_handler;
            boost::thread thread;

            Operation(ThreadPoolRef &_owner,
                      boost::function<void()> _function,
                      boost::function<void(const Error &err, const std::string &what)> _completion_handler)
                : owner(_owner)
                , function(std::move(_function))
                , completion_handler(std::move(_completion_handler))
            {
            }

            ~Operation()
            {
            }
        };

        class ThreadPoolRef::Functor
        {
        public:
            std::shared_ptr<Operation> operation_ptr;

            explicit Functor(std::shared_ptr<Operation> _operation_ptr)
                : operation_ptr(_operation_ptr)
            {
            }

            ~Functor()
            {
            }

            void operator()() const
            {
                Error error;
                std::string what;

                try
                {
                    operation_ptr->function();
                }
                catch (Exception &e)
                {
                    error = e.error();
                    if (!error)
                        error = std::errc::interrupted;

                    what = e.what();
                }
                catch (std::exception &e)
                {
                    error = std::errc::interrupted;
                    what = e.what();
                }
                catch (...)
                {
                    error = std::errc::interrupted;
                    what = "Unknown C++ exception occured";
                }

                operation_ptr->owner.GetProactor().ThreadSafePost(
                    boost::bind(
                        &ThreadPoolRef::HandleComplete,
                        &operation_ptr->owner,
                        operation_ptr,
                        error,
                        what));
            }
        };

        void ThreadPoolRef::RunOnThread(boost::function<void()> function,
                                        boost::function<void(const Error &err, const std::string &what)> completion_handler)
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);
            m_pending_ops++;

            std::shared_ptr<Operation> op(new Operation(*this, std::move(function), std::move(completion_handler)));

            op->thread = boost::thread(Functor(op));
        }

        void ThreadPoolRef::HandleComplete(std::shared_ptr<Operation> operation_ptr, const Error &error, const std::string &what)
        {
            BBOX_ASSERT((GetLocalRunLevel() == RunLevel::RUNNING) || (GetLocalRunLevel() == RunLevel::STOPPING));
            BBOX_ASSERT(m_pending_ops > 0);

            m_pending_ops--;

            if (operation_ptr->completion_handler)
                operation_ptr->completion_handler(error, what);

            CheckStopped();
        }

    } // namespace bbox::rt
} // namespace bbox
