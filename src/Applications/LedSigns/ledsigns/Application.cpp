/**
 * @file
 *
 * Main application for the LedSigns application.
 *
 */

#include <bbox/MainWrapper.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/CoutDebugTarget.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <bbox/rt/ConsoleShutdownService.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/Response.h>
#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <pion/http/response.hpp>
#include <pion/http/response_writer.hpp>
#include <pion/http/types.hpp>

#include <leds/FadecandyClient.h>
#include <leds/GpioClient.h>

#include <ledsigns/RenderService.h>

namespace ledsigns {

class ApplicationService : public bbox::rt::Service
{
public:

    ApplicationService(const std::string &name, 
                       bbox::rt::Proactor &parent,
                       const std::string &mode,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint,
                       const bbox::rt::net::TcpEndpoint &fadecandy_remote_endpoint)
        : bbox::rt::Service(name, parent)
        , m_http_listen_endpoint(http_listen_endpoint)
        , m_console_shutdown_service("console-shutdown-service", *this)
        , m_http_server("http-server", *this)
        , m_http_debug_website("http-debug-website", *this, m_http_server)
        , m_fadecandy_client("fadecandy-client", *this, fadecandy_remote_endpoint)
        , m_gpio_client("gpio-client", *this)
        , m_render_service("render-service", *this, mode, m_fadecandy_client, m_gpio_client)
    {
    }

    void HandleStarting() override
    {
        srand(unsigned(time(0)));

        m_http_server.AddServer(m_http_listen_endpoint);

        NotifyStarted();
    }

    void HandleStopping() override
    {
        RequestStopAllChildren();
        NotifyStopped();
    }

    void PrintState(bbox::DebugOutput &out) const override
    {
        out.Format("HTTP Listen Endpoint: %s\n", m_http_listen_endpoint.ToString());
    }

    bbox::rt::net::TcpEndpoint m_http_listen_endpoint;
    bbox::rt::ConsoleShutdownService m_console_shutdown_service;
    bbox::http::server::HttpServer m_http_server;
    bbox::http::debug::HttpDebugWebsite m_http_debug_website;
    leds::FadecandyClient m_fadecandy_client;
    leds::GpioClient m_gpio_client;
    RenderService m_render_service;

}; // class ApplicationService

} // namespace ledsigns

int gaysign_main(int argc, char *argv[])
{
	bbox::rt::net::TcpEndpoint http_local_endpoint;
    bbox::rt::net::TcpEndpoint fadecandy_remote_endpoint;

    if ((argc != 4)
		|| !bbox::rt::net::TcpEndpoint::FromString(argv[2], http_local_endpoint)
		|| !http_local_endpoint.GetAddress().is_v4()
		|| http_local_endpoint.GetAddress().is_multicast()
		|| (http_local_endpoint.GetPort() == 0)
        || !bbox::rt::net::TcpEndpoint::FromString(argv[3], fadecandy_remote_endpoint)
        || !fadecandy_remote_endpoint.GetAddress().is_v4()
        || fadecandy_remote_endpoint.GetAddress().is_multicast()
        || (fadecandy_remote_endpoint.GetPort() == 0)
        || ((0 != strcmp(argv[1], "gaysign")) && (0 != strcmp(argv[1], "casadelshade"))))
    {
        std::cerr << "Usage:  " << argv[0] << " mode ipv4-listen-addr:port fadecandy-server-addr:port" << std::endl;
		std::cerr << "   e.g. " << argv[0] << " gaysign 127.0.0.1:8080 127.0.0.1:7890" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Mode can be one of:" << std::endl;
        std::cerr << "    gaysign" << std::endl;
        std::cerr << "    casadelshade" << std::endl;
        return 1;
    }

	bbox::debug::DebugProvider debug_provider;
	bbox::debug::CoutDebugTarget console_target;
    bbox::rt::Proactor proactor;
    ledsigns::ApplicationService app("app", proactor, argv[1], http_local_endpoint, fadecandy_remote_endpoint);

    proactor.Run();

    return 0;
}

BBOX_MAIN_WRAPPER(gaysign_main)
