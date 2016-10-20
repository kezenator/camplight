/**
 * @file
 *
 * Header file for the camplight::pattern::TransitionPattern class.
 */

#ifndef __CAMPLIGHT__PATTERN__TRANSITION_PATTERN_H__
#define __CAMPLIGHT__PATTERN__TRANSITION_PATTERN_H__

#include <camplight/render/Pattern.h>
#include <camplight/render/Transition.h>
#include <bbox/rt/TimeSpan.h>
#include <memory>

namespace camplight
{
    namespace pattern
    {

        /**
         * A pattern that manages a transition from one pattern to the next.
         */
        class TransitionPattern : public render::Pattern
        {
        public:
            explicit TransitionPattern();
            ~TransitionPattern();

            void ChangeTo(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<render::Transition> &&transition);
            void ChangeToImmediate(std::unique_ptr<Pattern> &&pattern);

            std::string GetDescription() const override; 
            void RenderTo(render::ColorGrid &dest, render::Timestamp timestamp) override;

        private:
            struct Step
            {
                Step(std::unique_ptr<Pattern> &&_pattern, std::unique_ptr<render::Transition> &&_transition)
                    : pattern(std::move(_pattern))
                    , transition(std::move(_transition))
                {
                }

                std::unique_ptr<Pattern> pattern;
                std::unique_ptr<render::Transition> transition;
            };

            render::Timestamp m_last_render;
            render::Timestamp m_current_start;
            render::Timestamp m_next_start;

            std::unique_ptr<Pattern> m_current_ptr;
            std::unique_ptr<Step> m_next_ptr;
            std::unique_ptr<Step> m_pending_ptr;

            render::ColorGrid m_tmp_current_dest;
            render::ColorGrid m_tmp_next_dest;
        };

    } // namespace camplight::pattern
} // namespace camplight

#endif // __CAMPLIGHT__PATTERN__TRANSITION_PATTERN_H__
