/**
 * @file
 *
 * Main application for the GaySign application.
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
#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <pion/http/response.hpp>
#include <pion/http/response_writer.hpp>
#include <pion/http/types.hpp>

#include <leds/FadecandyClient.h>

#include <gaysign/RenderService.h>

namespace gaysign {

class ApplicationService : public bbox::rt::Service
{
public:

    ApplicationService(const std::string &name, 
                       bbox::rt::Proactor &parent,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint,
                       const bbox::rt::net::TcpEndpoint &fadecandy_remote_endpoint)
        : bbox::rt::Service(name, parent)
        , m_http_listen_endpoint(http_listen_endpoint)
        , m_console_shutdown_service("console-shutdown-service", *this)
        , m_http_server("http-server", *this)
        , m_fadecandy_client("fadecandy-client", *this, fadecandy_remote_endpoint)
        , m_render_service("render-service", *this, m_fadecandy_client)
    {
    }

    void HandleStarting() override
    {
        srand(unsigned(time(0)));

        m_http_server.AddServer(
            m_http_listen_endpoint,
            boost::bind(&ApplicationService::HttpRequestHandler, this, _1));

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

    void HttpRequestHandler(bbox::http::Request &request)
    {
        if (request.GetResource().substr(0, 6) == "/debug")
        {
            request.RespondWithDebugPage("/debug");
            return;
        }

        request.RespondWithNotFoundError();
    }

    bbox::rt::net::TcpEndpoint m_http_listen_endpoint;
    bbox::rt::ConsoleShutdownService m_console_shutdown_service;
    bbox::http::server::HttpServer m_http_server;
    leds::FadecandyClient m_fadecandy_client;
    RenderService m_render_service;

}; // class ApplicationService

} // namespace camplight

int gaysign_main(int argc, char *argv[])
{
	bbox::rt::net::TcpEndpoint http_local_endpoint;
    bbox::rt::net::TcpEndpoint fadecandy_remote_endpoint;

    bbox::rt::net::TcpEndpoint::FromString("192.168.0.22:7890", fadecandy_remote_endpoint);

    if ((argc != 2)
		|| !bbox::rt::net::TcpEndpoint::FromString(argv[1], http_local_endpoint)
		|| !http_local_endpoint.GetAddress().is_v4()
		|| http_local_endpoint.GetAddress().is_multicast()
		|| (http_local_endpoint.GetPort() == 0))
    {
        std::cerr << "Usage:  " << argv[0] << " ipv4-listen-addr:port" << std::endl;
		std::cerr << "   e.g. " << argv[0] << " 127.0.0.1:8080" << std::endl;
        return 1;
    }

	bbox::debug::DebugProvider debug_provider;
	bbox::debug::CoutDebugTarget console_target;
    bbox::rt::Proactor proactor;
    gaysign::ApplicationService app("app", proactor, http_local_endpoint, fadecandy_remote_endpoint);

    proactor.Run();

    return 0;
}

BBOX_MAIN_WRAPPER(gaysign_main)
