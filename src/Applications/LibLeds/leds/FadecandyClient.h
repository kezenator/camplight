/**
 * @file
 *
 * Header file for the leds::FadecandyClient class.
 */

#ifndef __LEDS_FADECANDY_CLIENT_H__
#define __LEDS_FADECANDY_CLIENT_H__

#include <bbox/rt/Service.h>
#include <bbox/rt/net/TcpEndpoint.h>

#include <leds/Color.h>
#include <leds/HsvColor.h>

#include <vector>

namespace leds
{

    /**
     * A class that maintains a connection to the Fadecandy
     * server and sends LED data to it when requested.
     */
    class FadecandyClient : public bbox::rt::Service
    {
    public:
        FadecandyClient(const std::string &name, Service &parent, bbox::rt::net::TcpEndpoint remote_endpoint);
        ~FadecandyClient();

        void UpdateLeds(const std::vector<Color> &colors);

    protected:
        void HandleStarting() override;
        void HandleStopping() override;
        void PrintState(bbox::DebugOutput &out) const override;

    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> m_pimpl;
    };

} // namespace leds

#endif // __LEDS_FADECANDY_CLIENT_H__
