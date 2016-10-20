/**
 * @file
 *
 * Implementation file for the camplight::pattern::TransitionPattern class.
 */

#include <camplight/pattern/TransitionPattern.h>
#include <camplight/pattern/Solid.h>
#include <bbox/Format.h>

namespace camplight
{
    namespace pattern
    {

        TransitionPattern::TransitionPattern()
            : m_last_render()
            , m_current_start()
            , m_next_start()
            , m_current_ptr(std::make_unique<Solid>(render::Color(0, 0, 0)))
            , m_next_ptr()
            , m_pending_ptr()
        {
        }

        TransitionPattern::~TransitionPattern()
        {
        }

        std::string TransitionPattern::GetDescription() const
        {
            if (!m_next_ptr)
                return m_current_ptr->GetDescription();
            else
            {
                return bbox::Format("Transition[from=%s, to=%s, transition=%s]",
                    m_current_ptr->GetDescription(),
                    m_next_ptr->pattern->GetDescription(),
                    m_next_ptr->transition->GetDescription());
            }
        }

        void TransitionPattern::ChangeTo(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<render::Transition> &&transition)
        {
            if (!m_next_ptr)
            {
                m_next_ptr = std::make_unique<Step>(std::move(pattern), std::move(transition));
                m_next_start = m_last_render;
            }
            else
            {
                m_pending_ptr = std::make_unique<Step>(std::move(pattern), std::move(transition));
            }
        }

        void TransitionPattern::ChangeToImmediate(std::unique_ptr<Pattern> &&pattern)
        {
            m_current_ptr = std::move(pattern);
            m_current_start = m_last_render;

            m_next_ptr.reset();
            m_pending_ptr.reset();
        }

        void TransitionPattern::RenderTo(render::ColorGrid &dest, render::Timestamp timestamp)
        {
            if (!m_next_ptr)
            {
                m_current_ptr->RenderTo(dest, timestamp.RelativeToStart(m_current_start));
            }
            else
            {
                m_current_ptr->RenderTo(m_tmp_current_dest, timestamp.RelativeToStart(m_current_start));
                m_next_ptr->pattern->RenderTo(m_tmp_next_dest, timestamp.RelativeToStart(m_next_start));

                render::Transition::Result result = m_next_ptr->transition->RenderTransition(
                    m_tmp_current_dest,
                    m_tmp_next_dest,
                    dest,
                    timestamp.RelativeToStart(m_next_start));

                switch (result)
                {
                case render::Transition::TransitionProgressing:
                    // Just keep going...
                    break;

                case render::Transition::TransitionComplete:
                    // The next becomes the current, and the pending
                    // (if any) becomes the next.

                    m_current_ptr = std::move(m_next_ptr->pattern);
                    m_current_start = m_next_start;

                    m_next_ptr = std::move(m_pending_ptr);
                    m_next_start = timestamp;

                    m_pending_ptr.reset();
                    break;
                }
            }

            m_last_render = timestamp;
        }

    } // namespace camplight::pattern
} // namespace camplight
