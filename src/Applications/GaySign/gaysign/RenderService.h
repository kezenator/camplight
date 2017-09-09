/**
 * @file
 *
 * Header file for the gaysign::RenderService class.
 *
 */

#ifndef __GAYSIGN__RENDER_SERVICE_H__
#define __GAYSIGN__RENDER_SERVICE_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <leds/FadecandyClient.h>

namespace gaysign
{

    class RenderService : public bbox::rt::Service
    {
    public:

        RenderService(const std::string &name,
            bbox::rt::Service &parent,
            leds::FadecandyClient &fadecandy_client);

        void HandleStarting() override;
        void HandleStopping() override;
        void PrintState(bbox::DebugOutput &out) const override;

    private:
        void HandleFrameTimeout();

        leds::FadecandyClient &m_fadecandy_client;
        bbox::rt::Timer m_frame_timer;

    }; // class RenderService

} // namespace camplight

#endif // __GAYSIGN__RENDER_SERVICE_H__
