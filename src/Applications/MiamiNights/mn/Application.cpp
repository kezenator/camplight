/**
 * @file
 *
 * Main application for the MiamiNights application.
 *
 */

#include <bbox/MainWrapper.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/CoutDebugTarget.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/rt/ConsoleShutdownService.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/http/Response.h>
#include <bbox/audio/AudioService.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <mn/Resources.h>

namespace mn {

class ApplicationService : public bbox::rt::Service
{
public:

    ApplicationService(const std::string &name, 
                       bbox::rt::Proactor &parent,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint)
        : bbox::rt::Service(name, parent)
        , m_http_listen_endpoint(http_listen_endpoint)
        , m_console_shutdown_service("console-shutdown-service", *this)
        , m_http_server("http-server", *this)
        , m_http_debug_website("http-debug-website", *this, m_http_server)
		, m_audio_service("audio service", *this)
    {
    }

private:

    void HandleStarting() override
    {
        m_http_server.AddServer(m_http_listen_endpoint,
            std::bind(&ApplicationService::HttpRequestHandler, this, std::placeholders::_1));

		//m_http_server.TryAndOpenWebBrowserToServer();

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
        if (request.RespondWithResource(g_resource_files))
        {
            return;
        }

        if (request.GetResource() == "/")
        {
            request.RespondWithTemporaryRedirect("/index.html");
            return;
        }

        request.RespondWithNotFoundError();
    }


    bbox::rt::net::TcpEndpoint m_http_listen_endpoint;
    bbox::rt::ConsoleShutdownService m_console_shutdown_service;
    bbox::http::server::HttpServer m_http_server;
    bbox::http::debug::HttpDebugWebsite m_http_debug_website;
	bbox::audio::AudioService m_audio_service;

}; // class ApplicationService

} // namespace camplight

int miami_nights_main(int argc, char *argv[])
{
	bbox::rt::net::TcpEndpoint endpoint;

	bbox::rt::net::TcpEndpoint::FromString("127.0.0.1:8080", endpoint);

	if (argc <= 1)
	{
		// OK - use default endpoint
	}
    else if ((argc >= 2)
		|| !bbox::rt::net::TcpEndpoint::FromString(argv[1], endpoint)
		|| !endpoint.GetAddress().is_v4()
		|| endpoint.GetAddress().is_multicast()
		|| (endpoint.GetPort() == 0))
    {
        std::cerr << "Usage:  " << argv[0] << " [ipv4-listen-addr:port]" << std::endl;
		std::cerr << "   e.g. " << argv[0] << " 127.0.0.1:8080" << std::endl;
        return 1;
    }

	bbox::debug::DebugProvider debug_provider;
	bbox::debug::CoutDebugTarget console_target;
    bbox::rt::Proactor proactor;
    mn::ApplicationService app("app", proactor, endpoint);

    proactor.Run();

    return 0;
}

BBOX_MAIN_WRAPPER(miami_nights_main)
