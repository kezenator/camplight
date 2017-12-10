/**
 * @file
 *
 * Implementation file for the ledsigns::common::TransitionPattern class.
 */

#include <ledsigns/common/TransitionPattern.h>
#include <ledsigns/common/SolidPattern.h>

namespace ledsigns
{
    namespace common
    {

        TransitionPattern::TransitionPattern(const common::RenderState &render)
            : m_cur_pattern(std::make_unique<SolidPattern>(render, leds::Color(0, 0, 0)))
            , m_next_pattern()
            , m_next_transition()
            , m_pending_pattern_factory()
            , m_pending_transition_factory()
        {
        }

        TransitionPattern::~TransitionPattern()
        {
        }

        void TransitionPattern::TransitionTo(const Pattern::Factory &pattern_factory, const Transition::Factory &transition_factory)
        {
            m_pending_pattern_factory = pattern_factory;
            m_pending_transition_factory = transition_factory;
        }

        std::string TransitionPattern::GetName() const
        {
            if (m_next_pattern)
            {
                return bbox::Format("Transition from %s to %s using %s",
                    m_cur_pattern->GetName(),
                    m_next_pattern->GetName(),
                    m_next_transition->GetName());
            }

            return bbox::Format("Transition @ %s", m_cur_pattern->GetName());
        }
        
        void TransitionPattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Current Pattern: %s\n", m_cur_pattern->GetName());
            out.Format("Current Pattern Info:\n");
            m_cur_pattern->PrintInformation(out);
            out.Format("\n");

            if (m_next_pattern)
            {
                out.Format("Next Pattern: %s\n", m_next_pattern->GetName());
                out.Format("Next Pattern Info:\n");
                m_next_pattern->PrintInformation(out);
                out.Format("\n");

                out.Format("Transition: %s\n", m_next_transition->GetName());
                out.Format("Transition Info:\n");
                m_next_transition->PrintInformation(out);
                out.Format("\n");

                if (m_pending_pattern_factory)
                {
                    out.Format("Plus there is another pending transition to apply...\n");
                }
            }
            else
            {
                out.Format("No transition active\n");
            }
        }

        std::vector<leds::Color> TransitionPattern::Render(const common::RenderState &render)
        {
            if (!m_next_pattern
                && m_pending_pattern_factory)
            {
                m_next_pattern = m_pending_pattern_factory(render);
                m_next_transition = m_pending_transition_factory(render);

                m_pending_pattern_factory = nullptr;
                m_pending_transition_factory = nullptr;
            }

            if (m_next_pattern)
            {
                // Need to fade between them

                auto cur_result = m_cur_pattern->Render(render);
                auto next_result = m_next_pattern->Render(render);
                auto trans_result = m_next_transition->Render(render);

                for (size_t i = 0; i < render.layout.num_leds; ++i)
                {
                    cur_result[i] = cur_result[i].Fade(next_result[i], trans_result[i]);
                }

                // Move to the next if the transition has completed

                if (m_next_transition->IsTransitionComplete())
                {
                    m_cur_pattern = std::move(m_next_pattern);
                    m_next_pattern = nullptr;
                    m_next_transition = nullptr;

                    if (m_pending_pattern_factory)
                    {
                        m_next_pattern = m_pending_pattern_factory(render);
                        m_next_transition = m_pending_transition_factory(render);

                        m_pending_pattern_factory = nullptr;
                        m_pending_transition_factory = nullptr;
                    }
                }

                return cur_result;
            }
            else // just cur pattern
            {
                return m_cur_pattern->Render(render);
            }
        }

    } // namespace ledsigns::common
} // namespace ledsigns
