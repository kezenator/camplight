/**
 * @file
 *
 * Implementation file for the gaysign::RenderService class.
 *
 */

#include <ledsigns/RenderService.h>

#include <ledsigns/common/CyclePattern.h>
#include <ledsigns/common/FadeTransition.h>

#include <ledsigns/casadelshade/CasaDelShadeLayout.h>
#include <ledsigns/casadelshade/FlickeringNeonPattern.h>
#include <ledsigns/casadelshade/StaggerPattern.h>

#include <ledsigns/gaysign/GaySignLayout.h>

namespace ledsigns
{                     
                                
    RenderService::RenderService(const std::string &name,
                                    bbox::rt::Service &parent,
                                    leds::FadecandyClient &fadecandy_client,
                                    leds::GpioClient &gpio_client)
        : bbox::rt::Service(name, parent)
        , m_fadecandy_client(fadecandy_client)
        , m_gpio_client(gpio_client)
        , m_frame_timer("frame-timer", *this)
    {
        m_frame_timer.SetHandler(std::bind(&RenderService::HandleFrameTimeout, this));

        SetThisDependantOn(fadecandy_client);
        SetThisDependantOn(gpio_client);
    }

    void RenderService::HandleStarting()
    {
        m_render_state_ptr = std::make_unique<common::RenderState>(
            casadelshade::CasaDelShadeLayout,
            0);

        m_pattern_ptr = std::make_unique<common::CyclePattern>(
            *m_render_state_ptr,
            std::vector<common::Pattern::Factory>({
                casadelshade::StaggerPattern::Factory(),
                casadelshade::FlickeringNeonPattern::Factory(),
            }),
            std::vector<common::Transition::Factory>({
                common::FadeTransition::Factory(500),
            }),
            10000);

        m_frame_timer.StartPeriodic(
            bbox::rt::TimeSpan::Milliseconds(30),
            bbox::rt::TimeSpan::Milliseconds(30));

        NotifyStarted();
    }

    void RenderService::HandleStopping()
    {
        m_frame_timer.Cancel();

        RequestStopAllChildren();
        NotifyStopped();
    }

    void RenderService::PrintState(bbox::DebugOutput &out) const
    {
        out.Format("Layout Name:   %s\n", m_render_state_ptr->layout.name);
        out.Format("Layout # LEDs: %d\n", m_render_state_ptr->layout.num_leds);
        out.Format("Time (ms):     %d\n", m_render_state_ptr->time_ms);
        out.Format("Pattern Name:  %s\n", m_pattern_ptr->GetName());
        out.Format("Pattern State:\n");
        m_pattern_ptr->PrintInformation(out);
    }

    void RenderService::HandleFrameTimeout()
    {
        m_gpio_client.SetOutput(BUTTON_OUTPUT, !m_gpio_client.GetInput(BUTTON_INPUT));

        m_render_state_ptr->time_ms += 30;

        m_fadecandy_client.UpdateLeds(
            m_pattern_ptr->Render(*m_render_state_ptr));
    }

} // namespace ledsigns
