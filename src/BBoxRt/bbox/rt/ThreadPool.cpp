/**
* @file
*
* Implementation for the bbox::rt::ThreadPool class.
*/

#include <bbox/rt/ThreadPool.h>
#include <bbox/Assert.h>
#include <bbox/Exception.h>

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace bbox {
    namespace rt {

        const char *ThreadPool::SERVICE_NAME = "bbox::rt::ThreadPool";

        struct ThreadPool::Pimpl
        {
        public:

            using func_pair = std::pair<boost::function<void()>, std::function<void(const Error &err, const std::string &what)>>;

            explicit Pimpl(ThreadPool &parent)
                : m_parent(parent)
                , m_proactor(parent.GetProactor())
                , m_thread()
                , m_mutex()
                , m_cond_var()
                , m_queue()
                , m_finished(false)
                , m_items_in_progress(0)
                , m_items_completed(0)
            {
                m_thread = std::thread(std::bind(&Pimpl::ThreadFunc, this));
            }

            ~Pimpl()
            {
                m_thread.join();
            }

            void HandleStopping()
            {
                {
                    std::unique_lock<std::mutex> lock(m_mutex);

                    BBOX_ASSERT(!m_finished);

                    m_finished = true;
                    m_cond_var.notify_all();
                }
            }

            void Post(
                boost::function<void()> &&function,
                boost::function<void(const Error &err, const std::string &what)> &&completion_handler)
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                BBOX_ASSERT(!m_finished);

                m_queue.emplace_back(std::move(function), std::move(completion_handler));
                m_items_in_progress += 1;

                m_cond_var.notify_all();
            }

            size_t GetItemsInProgress()
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                return m_items_in_progress;
            }

            size_t GetItemsCompleted()
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                return m_items_completed;
            }

        private:
            void ThreadFunc()
            {
                while (true)
                {
                    func_pair entry;

                    {
                        std::unique_lock<std::mutex> lock(m_mutex);

                        if (!m_queue.empty())
                        {
                            entry = std::move(m_queue.front());
                            m_queue.pop_front();
                        }
                        else if (m_finished)
                        {
                            break;
                        }
                        else
                        {
                            m_cond_var.wait(lock);
                            continue;
                        }
                    }

                    RunItem(entry);

                    {
                        std::unique_lock<std::mutex> lock(m_mutex);

                        m_items_in_progress -= 1;
                        m_items_completed += 1;
                    }
                }

                m_proactor.ThreadSafePost(std::bind(&ThreadPool::NotifyThreadCompleted, &m_parent));
            }

            void RunItem(func_pair &entry)
            {
                Error error;
                std::string what;

                try
                {
                    entry.first();
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

                m_proactor.ThreadSafePost(
                    [=]()
                    {
                        entry.second(error, what);
                    });
            }

            ThreadPool &m_parent;
            Proactor &m_proactor;
            std::thread m_thread;

            std::mutex m_mutex;
            std::condition_variable m_cond_var;
            std::deque<func_pair> m_queue;
            bool m_finished;
            size_t m_items_in_progress;
            size_t m_items_completed;
        };

        ThreadPool::ThreadPool(const std::string &name, Proactor &parent)
            : Service(name, parent)
            , m_pimpl()
        {
            RegisterService(SERVICE_NAME, this);
        }

        ThreadPool::ThreadPool(const std::string &name, Service &parent)
            : Service(name, parent)
            , m_pimpl()
        {
            RegisterService(SERVICE_NAME, this);
        }

        ThreadPool::~ThreadPool()
        {
        }

        void ThreadPool::HandleStarting()
        {
            m_pimpl = std::make_unique<Pimpl>(*this);

            NotifyStarted();
        }

        void ThreadPool::HandleStopping()
        {
            m_pimpl->HandleStopping();
        }

        void ThreadPool::PrintState(bbox::DebugOutput &out) const
        {
            if (m_pimpl)
            {
                out.Format("Items In Progress: %d\n", m_pimpl->GetItemsInProgress());
                out.Format("Items Completed:   %d\n", m_pimpl->GetItemsCompleted());
            }
        }

        void ThreadPool::Post(
            boost::function<void()> &&function,
            boost::function<void(const Error &err, const std::string &what)> &&completion_handler)
        {
            BBOX_ASSERT(GetLocalRunLevel() == RunLevel::RUNNING);

            m_pimpl->Post(
                std::move(function),
                std::move(completion_handler));
        }

        void ThreadPool::NotifyThreadCompleted()
        {
            m_pimpl = nullptr;

            NotifyStopped();
            RequestStopAllChildren();
        }

    } // namespace bbox::rt
} // namespace bbox
