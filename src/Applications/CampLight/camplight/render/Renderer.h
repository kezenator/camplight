/**
 * @file
 *
 * Header file for the camplight::render::Renderer class.
 */

#ifndef __CAMPLIGHT__RENDER__RENDERER_H__
#define __CAMPLIGHT__RENDER__RENDERER_H__

#include <camplight/factory/PatternFactory.h>
#include <camplight/factory/TransitionFactory.h>
#include <camplight/factory/SequenceFactory.h>
#include <camplight/render/Sequence.h>

#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <memory>

namespace camplight
{
    namespace render
    {

        /**
         * The class that controls the rendering.
         */
        class Renderer: public bbox::rt::Service
        {
        public:
            Renderer(const std::string &name, bbox::rt::Service &parent);
            ~Renderer();

            factory::PatternFactory &GetPatternFactory() { return m_pattern_factory; }
            factory::TransitionFactory &GetTransitionFactory() { return m_transition_factory; }
            factory::SequenceFactory &GetSequenceFactory() { return m_sequence_factory; }

            void GetLedsForWebDisplay(uint32_t &width, uint32_t &height, std::vector<Color> &main_leds, std::vector<Color> &top_leds) const;

            void ChangeToSequence(const std::string &sequence_name);

        protected:

            // Implements bbox::rt::Service
            void HandleStarting() override;
            void HandleStopping() override;
            void PrintState(bbox::DebugOutput &out) const override;

        private:

            void HandleTimeout();
            void Render(Timestamp timestamp, bool wait_first);

            static constexpr size_t RENDER_PERIOD_MS = 33;

            struct Hardware;

            bbox::rt::Timer m_timer;

            factory::PatternFactory m_pattern_factory;
            factory::TransitionFactory m_transition_factory;
            factory::SequenceFactory m_sequence_factory;

            Timestamp m_timestamp;
            Sequence m_sequence;

            std::unique_ptr<Hardware> m_hardware_ptr;

            ColorGrid m_main_rendered;
            ColorGrid m_top_rendered;
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__RENDERER_H__
