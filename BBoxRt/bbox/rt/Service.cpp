/**
* @file
*
* Implementation for the bbox::rt::Service class.
*/

#include <bbox/rt/Service.h>
#include <bbox/rt/Proactor.h>

namespace bbox {
    namespace rt {

        Service::Service(const std::string &name, Proactor &parent)
            : details::ResourceBase(name, parent, RunLevel::CONSTRUCTED)
        {
        }

        Service::Service(const std::string &name, Service &parent)
            : details::ResourceBase(name, parent, RunLevel::CONSTRUCTED)
        {
        }

        Service::~Service()
        {
        }

    } // namespace bbox::rt
} // namespace bbox
