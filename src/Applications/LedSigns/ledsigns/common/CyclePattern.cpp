/**
 * @file
 *
 * Implementation file for the ledsigns::common::CyclePattern class.
 */

#include <ledsigns/common/CyclePattern.h>

namespace ledsigns
{
    namespace common
    {

        CyclePattern::CyclePattern(const common::RenderState &render,
                                   std::vector<Pattern::Factory> pattern_factories,
                                   std::vector<Transition::Factory> transition_factories,
                                   uint64_t period_ms)
            : m_pattern_factories(std::move(pattern_factories))
            , m_transition_factories(std::move(transition_factories))
            , m_period_ms(period_ms)
            , m_next_transition_ms(render.time_ms)
            , m_last_pattern_index(std::numeric_limits<size_t>::max())
            , m_last_transition_index(std::numeric_limits<size_t>::max())
            , m_transition_pattern(render)
        {
        }

        CyclePattern::~CyclePattern()
        {
        }

        std::string CyclePattern::GetName() const
        {
            return bbox::Format("Cycle [%d patterns, %d transitions], currently: %s",
                m_pattern_factories.size(),
                m_transition_factories.size(),
                m_transition_pattern.GetName());
        }
        
        void CyclePattern::PrintInformation(bbox::DebugOutput &out) const
        {
            out.Format("Num Patterns:    %d [Last index: %d]\n", m_pattern_factories.size(), m_last_pattern_index);
            out.Format("Num Transitions: %d [Last index: %d]\n", m_transition_factories.size(), m_last_transition_index);
            out.Format("Period:          %d ms\n", m_period_ms);
            out.Format("Next Transition: %d ms\n", m_next_transition_ms);
            out.Format("Transition Pattern Info:\n");
            m_transition_pattern.PrintInformation(out);
        }

        std::vector<leds::Color> CyclePattern::Render(const common::RenderState &render)
        {
            if (render.time_ms >= m_next_transition_ms)
            {
                m_next_transition_ms = render.time_ms + m_period_ms;

                size_t next_pattern_index;
                size_t next_transition_index;

                do 
                {
                    next_pattern_index = rand() % m_pattern_factories.size();
                    next_transition_index = rand() % m_transition_factories.size();
                }
                while (((m_pattern_factories.size() > 1) && (next_pattern_index == m_last_pattern_index))
                    || ((m_transition_factories.size() > 1) && (next_transition_index == m_last_transition_index)));

                m_last_pattern_index = next_pattern_index;
                m_last_transition_index = next_transition_index;

                m_transition_pattern.TransitionTo(
                    m_pattern_factories[m_last_pattern_index],
                    m_transition_factories[m_last_transition_index]);
            }

            return m_transition_pattern.Render(render);
        }

    } // namespace ledsigns::common
} // namespace ledsigns
