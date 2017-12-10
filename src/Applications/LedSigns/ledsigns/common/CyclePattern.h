/**
 * @file
 *
 * Header file for the ledsigns::common::CyclePattern class.
 */

#ifndef __LEDSIGNS__COMMON__CYCLE_PATTERN_H__
#define __LEDSIGNS__COMMON__CYCLE_PATTERN_H__

#include <vector>
#include <ledsigns/common/TransitionPattern.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that cycles randomly between a set of patterns.
         */
        class CyclePattern : public common::Pattern
        {
        public:

            CyclePattern(
                const common::RenderState &render,
                std::vector<Pattern::Factory> pattern_factories,
                std::vector<Transition::Factory> transition_factories,
                uint64_t period_ms);

            ~CyclePattern();

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            const std::vector<Pattern::Factory> m_pattern_factories;
            const std::vector<Transition::Factory> m_transition_factories;
            const uint64_t m_period_ms;

            uint64_t m_next_transition_ms;

            size_t m_last_pattern_index;
            size_t m_last_transition_index;

            TransitionPattern m_transition_pattern;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__CYCLE_PATTERN_H__
