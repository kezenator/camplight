/**
 * @file
 *
 * Header file for the camplight::render::Renderer class.
 */

#ifndef __CAMPLIGHT__RENDER__RENDERER_H__
#define __CAMPLIGHT__RENDER__RENDERER_H__

#include <camplight/pattern/TransitionPattern.h>

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

            void GetLedsForWebDisplay(uint32_t &width, uint32_t &height, std::vector<Color> &main_leds, std::vector<Color> &top_leds) const;

            void ChangeMainPattern(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<Transition> &&transition);
            void ChangeTopPattern(std::unique_ptr<Pattern> &&pattern, std::unique_ptr<Transition> &&transition);

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

            Timestamp m_timestamp;
            std::unique_ptr<pattern::TransitionPattern> m_main_pattern_ptr;
            std::unique_ptr<pattern::TransitionPattern> m_top_pattern_ptr;

            std::unique_ptr<Hardware> m_hardware_ptr;

            ColorGrid m_main_rendered;
            ColorGrid m_top_rendered;
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__RENDERER_H__
