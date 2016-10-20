/**
* @file
*
* Implementation for the bbox::http::pionhlp::Scheduler class.
*/

#include <bbox/http/pionhlp/Scheduler.h>

namespace bbox {
    namespace http {
        namespace pionhlp {

            Scheduler::Scheduler(const std::string &name, rt::Service &parent)
                : rt::Resource(name, parent)
            {
            }

            Scheduler::Scheduler(const std::string &name, rt::Resource &parent)
                : rt::Resource(name, parent)
            {
            }

            Scheduler::~Scheduler()
            {
            }

            boost::asio::io_service &Scheduler::get_io_service()
            {
                return GetProactor().GetIoService();
            }

            void Scheduler::HandleStopping()
            {
                NotifyStopped();
                RequestStopAllChildren();
            }

			void Scheduler::PrintState(bbox::DebugOutput & /*out*/) const
			{
			}

        } // namespace bbox::http::pionhlp
    } // namespace bbox::http
} // namespace bbox
