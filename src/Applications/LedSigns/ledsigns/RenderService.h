/**
 * @file
 *
 * Header file for the ledsigns::RenderService class.
 *
 */

#ifndef __LEDSIGNS__RENDER_SERVICE_H__
#define __LEDSIGNS__RENDER_SERVICE_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <leds/FadecandyClient.h>
#include <leds/GpioClient.h>
#include <ledsigns/common/Pattern.h>

namespace ledsigns
{

    class RenderService : public bbox::rt::Service
    {
    public:

        RenderService(const std::string &name,
            bbox::rt::Service &parent,
            const std::string &mode,
            leds::FadecandyClient &fadecandy_client,
            leds::GpioClient &gpio_client);

        void HandleStarting() override;
        void HandleStopping() override;
        void PrintState(bbox::DebugOutput &out) const override;

    private:

        static constexpr size_t BUTTON_INPUT = 17;
        static constexpr size_t BUTTON_OUTPUT = 27;

        void HandleFrameTimeout();

        const std::string m_mode;
        leds::FadecandyClient &m_fadecandy_client;
        leds::GpioClient &m_gpio_client;
        bbox::rt::Timer m_frame_timer;
        std::unique_ptr<common::RenderState> m_render_state_ptr;
        common::Pattern::Ptr m_pattern_ptr;

    }; // class RenderService

} // namespace ledsigns

#endif // __LEDSIGNS__RENDER_SERVICE_H__
