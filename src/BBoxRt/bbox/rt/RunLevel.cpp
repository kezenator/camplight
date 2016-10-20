/**
 * @file
 *
 * Implementation for the bbox::rt::RunLevel class.
 */

#include <bbox/rt/RunLevel.h>

namespace bbox {
    namespace rt { 

        std::string RunLevel::ToString() const
        {
            switch (m_level)
            {
            case CONSTRUCTED:   return "Constructed";
            case STARTING:      return "Starting";
            case RUNNING:       return "Running";
            case STOPPING:      return "Stopping";
            case STOPPED:       return "Stopped";
            }

            return "Unknown";
        }

    } // namespace bbox::rt
} // namespace bbox
