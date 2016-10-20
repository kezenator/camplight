/**
* @file
*
* Implementation for the bbox::rt::Resource class.
*/

#include <bbox/Assert.h>
#include <bbox/rt/Resource.h>
#include <bbox/rt/Service.h>

namespace bbox {
    namespace rt {

        Resource::Resource(const std::string &name, Service &parent)
            : details::ResourceBase(name, parent, RunLevel::RUNNING)
        {
        }

        Resource::Resource(const std::string &name, Resource &parent)
            : details::ResourceBase(name, parent, RunLevel::RUNNING)
        {
        }
        
        Resource::~Resource()
        {
        }

        void Resource::HandleStarting()
        {
            // Resources should never get here
            BBOX_ASSERT(false);
        }

    } // namespace bbox::rt
} // namespace bbox
