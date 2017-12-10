/**
 * @file
 *
 * Header file for the ledsigns::common::TransitionPattern class.
 */

#ifndef __LEDSIGNS__COMMON__TRANSITION_PATTERN_H__
#define __LEDSIGNS__COMMON__TRANSITION_PATTERN_H__

#include <ledsigns/common/Pattern.h>
#include <ledsigns/common/Transition.h>

namespace ledsigns
{
    namespace common
    {

        /**
         * A pattern that transitions (when requested) from
         * on pattern to another using a specified transition.
         */
        class TransitionPattern : public common::Pattern
        {
        public:

            explicit TransitionPattern(const common::RenderState &render);
            ~TransitionPattern();

            void TransitionTo(const Pattern::Factory &pattern_factory, const Transition::Factory &transition_factory);

            std::string GetName() const override;
            void PrintInformation(bbox::DebugOutput &out) const override;

            std::vector<leds::Color> Render(const common::RenderState &render) override;

        private:
            Pattern::Ptr m_cur_pattern;
            Pattern::Ptr m_next_pattern;
            Transition::Ptr m_next_transition;
            Pattern::Factory m_pending_pattern_factory;
            Transition::Factory m_pending_transition_factory;
        };

    } // namespace ledsigns::common
} // namespace ledsigns

#endif // __LEDSIGNS__COMMON__TRANSITION_PATTERN_H__
