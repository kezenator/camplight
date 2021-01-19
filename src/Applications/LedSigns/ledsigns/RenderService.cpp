/**
 * @file
 *
 * Implementation file for the gaysign::RenderService class.
 *
 */

#include <ledsigns/RenderService.h>

#include <ledsigns/common/ColorSets.h>
#include <ledsigns/common/CountPattern.h>
#include <ledsigns/common/CyclePattern.h>
#include <ledsigns/common/GrowingCirclesPattern.h>
#include <ledsigns/common/FadeTransition.h>
#include <ledsigns/common/NoisePattern.h>
#include <ledsigns/common/PulsePattern.h>
#include <ledsigns/common/SolidPattern.h>
#include <ledsigns/common/StaggerPattern.h>
#include <ledsigns/common/SwipeTestPattern.h>
#include <ledsigns/common/SwipeTransition.h>
#include <ledsigns/common/WavePattern.h>
#include <ledsigns/common/VectorGradient.h>

#include <ledsigns/casadelshade/CasaDelShadeLayout.h>
#include <ledsigns/casadelshade/FlickeringNeonPattern.h>

#include <ledsigns/gaysign/GaySignLayout.h>
#include <ledsigns/gaysign/GaySignPattern.h>

#include <ledsigns/buttonbox/ButtonBoxLayout.h>
#include <ledsigns/buttonbox/ButtonBoxPattern.h>

namespace ledsigns
{                     
                                
    RenderService::RenderService(const std::string &name,
                                 bbox::rt::Service &parent,
                                 const std::string &mode,
                                 leds::FadecandyClient &fadecandy_client,
                                 leds::GpioClient &gpio_client)
        : bbox::rt::Service(name, parent)
        , m_mode(mode)
        , m_fadecandy_client(fadecandy_client)
        , m_gpio_client(gpio_client)
        , m_frame_timer("frame-timer", *this)
    {
        m_frame_timer.SetHandler(std::bind(&RenderService::HandleFrameTimeout, this));

        SetThisDependantOn(fadecandy_client);
        SetThisDependantOn(gpio_client);

        if (m_mode == "gaysign")
        {
            m_render_state_ptr = std::make_unique<common::RenderState>(
                gaysign::GaySignLayout,
                0);

            m_pattern_ptr = std::make_unique<gaysign::GaySignPattern>(
                m_gpio_client,
                *m_render_state_ptr,
                10 * 1000,
                2 * 1000);
        }
        else if (m_mode == "gaysign-party")
        {
            m_render_state_ptr = std::make_unique<common::RenderState>(
                gaysign::GaySignLayout,
                0);

            m_pattern_ptr = std::make_unique<common::CyclePattern>(
                *m_render_state_ptr,
                std::vector<common::Pattern::Factory>({
                    [&](const common::RenderState& render) { return std::make_unique<gaysign::GaySignPattern>(m_gpio_client, render, 500, 500); },
                    common::StaggerPattern::Factory(),
                    common::NoisePattern::Factory(common::Gradient::FullRainbow(), 0.75, 1000.0),
                    common::WavePattern::RandomDirectionFactory(3000, 6.0, common::Gradient::FullRainbow()),
                    common::PulsePattern::Factory(common::NoisePattern::Factory(common::Gradient::FullRainbow(), 1.5, 1500.0)),
                    }),
                std::vector<common::Transition::Factory>({
                    common::FadeTransition::Factory(1500),
                    common::SwipeTransition::RandomDirectionFactory(1500),
                }),
                90 * 1000);
        }
        else if (m_mode == "buttonbox")
        {
            m_render_state_ptr = std::make_unique<common::RenderState>(
                buttonbox::ButtonBoxLayout,
                0);

            m_pattern_ptr = std::make_unique<buttonbox::ButtonBoxPattern>(
                *this,
                m_gpio_client,
                *m_render_state_ptr);
        }
        else // casadelshade
        {
            m_render_state_ptr = std::make_unique<common::RenderState>(
                casadelshade::CasaDelShadeLayout,
                0);

            m_pattern_ptr = std::make_unique<common::CyclePattern>(
                *m_render_state_ptr,
                std::vector<common::Pattern::Factory>({
                    common::PulsePattern::Factory(common::NoisePattern::Factory(common::Gradient::CasaDelShadePulseReds(), 0.2, 350.0)),
                    common::StaggerPattern::Factory(),
                    casadelshade::FlickeringNeonPattern::Factory(),
                    //common::GrowingCirclesPattern::Factory(6000, 3.0, common::Gradient::FullRainbow()),
                    //common::WavePattern::RandomDirectionFactory(3000, 3.0, common::Gradient::FullRainbow()),
                    common::NoisePattern::Factory(common::Gradient::GreensAndBlues(), 0.13, 750.0),
                    common::NoisePattern::Factory(common::Gradient::FullRainbow(), 0.5, 1000.0),
                }),
                std::vector<common::Transition::Factory>({
                    common::FadeTransition::Factory(500),
                    common::SwipeTransition::RandomDirectionFactory(500),
                }),
                5 * 60 * 1000);
        }
    }

    void RenderService::HandleStarting()
    {
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
