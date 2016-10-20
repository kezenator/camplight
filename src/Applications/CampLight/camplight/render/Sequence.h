/**
 * @file
 *
 * Header file for the camplight::render::Sequence class.
 */

#ifndef __CAMPLIGHT__RENDER__SEQUENCE_H__
#define __CAMPLIGHT__RENDER__SEQUENCE_H__

#include <camplight/render/ColorGrid.h>
#include <camplight/render/Timestamp.h>
#include <camplight/render/Pattern.h>
#include <camplight/render/Transition.h>
#include <camplight/pattern/TransitionPattern.h>

#include <camplight/render/SequenceEntry.h>

#include <memory>
#include <boost/optional.hpp>

namespace camplight
{
    // Forward declarations
    namespace factory
    {
        class PatternFactory;
        class TransitionFactory;
    }

    namespace render
    {

        /**
         * A class that sequences patterns and transitions
         * for both the top and bottom LEDs arrays.
         */
        class Sequence
        {
            Sequence() = delete;
            Sequence(const Sequence &) = delete;
            Sequence &operator =(const Sequence &) = delete;

        public:

            explicit Sequence(factory::PatternFactory &pattern_factory,
                              factory::TransitionFactory &transition_factory);
            ~Sequence();

            void ChangeToBlackImmediate();
            void ChangeTo(std::vector<SequenceEntry> &&entries);

            void RenderTo(render::ColorGrid &top_dest, render::ColorGrid &main_dest, render::Timestamp timestamp);

        private:
            factory::PatternFactory &m_pattern_factory;
            factory::TransitionFactory &m_transition_factory;

            std::unique_ptr<pattern::TransitionPattern> m_main_pattern;
            std::unique_ptr<pattern::TransitionPattern> m_top_pattern;

            std::vector<SequenceEntry> m_entries;
            boost::optional<size_t> m_opt_cur_index;
            render::Timestamp m_cur_start_timestamp;
            render::Timestamp m_next_start_timestamp;
            render::Timestamp m_last_render_timestamp;
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__SEQUENCE_H__
