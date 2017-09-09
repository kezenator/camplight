/**
 * @file
 *
 * Header for the bbox::rt::DebugEnable class.
 */

#ifndef __BBOX__RT__DEBUG_ENABLE_H__
#define __BBOX__RT__DEBUG_ENABLE_H__

#include <bbox/rt/Resource.h>

namespace bbox {
    namespace rt { 

        /**
         * A debug enable that can be controlled via the debug website.
         */
        class DebugEnable : public Resource
        {
        public:
            DebugEnable(const std::string &name, Service &parent);
            DebugEnable(const std::string &name, Resource &parent);
            ~DebugEnable();

            bool DebugOutputEnabled() const { return m_enabled; }
            std::string DebugOutputReason() const { return Resource::GetResourceFullPath(); }

        private:
            // Implements Resource
            void HandleStopping() override;
            void DebugVisit(bbox::debug::DebugVisitor &&visitor) override;
            void PrintState(bbox::DebugOutput &out) const override;

            bool m_enabled;
        };

    } // namespace bbox::rt
} // namespace bbox

#endif // __BBOX__RT__DEBUG_ENABLE_H__