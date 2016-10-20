/**
 * @file
 *
 * Implementation file for the camplight::render::Sequence class.
 */

#include <camplight/render/Sequence.h>
#include <camplight/factory/PatternFactory.h>
#include <camplight/factory/TransitionFactory.h>

#include <camplight/pattern/Solid.h>

namespace camplight
{
    namespace render
    {

        Sequence::Sequence(factory::PatternFactory &pattern_factory,
                           factory::TransitionFactory &transition_factory)
            : m_pattern_factory(pattern_factory)
            , m_transition_factory(transition_factory)
            , m_main_pattern(std::make_unique<pattern::TransitionPattern>())
            , m_top_pattern(std::make_unique<pattern::TransitionPattern>())
            , m_entries()
            , m_opt_cur_index(boost::none)
            , m_cur_start_timestamp()
            , m_next_start_timestamp()
            , m_last_render_timestamp()
        {
        }

        Sequence::~Sequence()
        {
        }

        void Sequence::ChangeToBlackImmediate()
        {
            m_entries.clear();
            m_opt_cur_index = boost::none;

            m_main_pattern->ChangeToImmediate(std::make_unique<pattern::Solid>(Color(0, 0, 0)));
            m_top_pattern->ChangeToImmediate(std::make_unique<pattern::Solid>(Color(0, 0, 0)));
        }

        void Sequence::ChangeTo(std::vector<SequenceEntry> &&entries)
        {
            m_opt_cur_index = boost::none;
            m_entries = std::move(entries);
        }

        void Sequence::RenderTo(render::ColorGrid &top_dest, render::ColorGrid &main_dest, render::Timestamp timestamp)
        {
            if (!m_opt_cur_index
                && !m_entries.empty())
            {
                // We are able to start the first pattern

                m_opt_cur_index = 0;
                m_cur_start_timestamp = timestamp;
                m_next_start_timestamp = Timestamp(timestamp.ToMilliseconds() + m_entries[0].duration.ToMilliseconds());

                m_main_pattern->ChangeTo(
                    m_pattern_factory.CreatePattern(m_entries[0].main_pattern_name),
                    m_transition_factory.CreateTransition(m_entries[0].main_transition_name));

                m_top_pattern->ChangeTo(
                    m_pattern_factory.CreatePattern(m_entries[0].top_pattern_name),
                    m_transition_factory.CreateTransition(m_entries[0].top_transition_name));
            }
            else if (m_opt_cur_index
                && (timestamp.ToMilliseconds() >= m_next_start_timestamp.ToMilliseconds()))
            {
                // We need to move to the next entry

                size_t next_index = m_opt_cur_index.get() + 1;
                if (next_index >= m_entries.size())
                    next_index = 0;

                const SequenceEntry &entry = m_entries[next_index];

                m_opt_cur_index = next_index;
                m_cur_start_timestamp = timestamp;
                m_next_start_timestamp = Timestamp(timestamp.ToMilliseconds() + entry.duration.ToMilliseconds());

                m_main_pattern->ChangeTo(
                    m_pattern_factory.CreatePattern(entry.main_pattern_name),
                    m_transition_factory.CreateTransition(entry.main_transition_name));

                m_top_pattern->ChangeTo(
                    m_pattern_factory.CreatePattern(entry.top_pattern_name),
                    m_transition_factory.CreateTransition(entry.top_transition_name));
            }

            // Finally, just render the current state and
            // update the last rendered time

            m_main_pattern->RenderTo(main_dest, timestamp);
            m_top_pattern->RenderTo(top_dest, timestamp);

            m_last_render_timestamp = timestamp;
        }

    } // namespace camplight::render
} // namespace camplight
