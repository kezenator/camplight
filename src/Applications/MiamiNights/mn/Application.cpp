/**
 * @file
 *
 * Implementation file for the mn::Application class and the overall application.
 *
 */

#include <mn/Application.h>

#include <bbox/Format.h>
#include <bbox/Assert.h>

#include <bbox/http/Response.h>

#include <bbox/enc/ToXml.h>

#include <mn/Resources.h>

namespace mn {

ApplicationService::ApplicationService(const std::string &name,
                       bbox::rt::Proactor &parent,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint)
    : bbox::rt::Service(name, parent)
    , m_http_listen_endpoint(http_listen_endpoint)
    , m_console_shutdown_service("console-shutdown-service", *this)
    , m_http_server("http-server", *this)
    , m_http_debug_website("http-debug-website", *this, m_http_server)
	, m_audio_service("audio service", *this)
	, m_buttons_web_socket("buttons-web-socket", *this, m_http_server, "/ws/buttons", "kezenator.com/uri/protocols/ws/miami-nights-buttons/2018-09-12")
	, m_app_web_socket("app-web-socket", *this, m_http_server, "/ws/app", "kezenator.com/uri/protocols/ws/miami-nights-app/2018-09-12")
{
	SetThisDependantOn(m_http_server);

	m_buttons_web_socket.RegisterHandler(&ApplicationService::HandleButtonRxButtonState, this);
	m_app_web_socket.RegisterHandler(&ApplicationService::HandleAppRxButtonColors, this);
}

void ApplicationService::HandleStarting()
{
    m_http_server.AddServer(m_http_listen_endpoint,
        std::bind(&ApplicationService::HttpRequestHandler, this, std::placeholders::_1));

	m_http_server.TryAndOpenWebBrowserToServer();

    NotifyStarted();
}

void ApplicationService::HandleStopping()
{
    RequestStopAllChildren();
    NotifyStopped();
}

void ApplicationService::PrintState(bbox::DebugOutput &out) const
{
    out.Format("HTTP Listen Endpoint: %s\n", m_http_listen_endpoint.ToString());
	out.Format("Button States:\n");
	{
		out.IncIndent(4);
		out.Format("%s\n", bbox::enc::ToXml::ConvertToPretty("button-states", m_button_states));
		out.DecIndent(4);
	}
	out.Format("Button Colors:\n");
	{
		out.IncIndent(4);
		out.Format("%s\n", bbox::enc::ToXml::ConvertToPretty("button-colors", m_button_colors));
		out.DecIndent(4);
	}
}

void ApplicationService::HttpRequestHandler(bbox::http::Request &request)
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

void ApplicationService::HandleButtonRxButtonState(const msgs::ButtonStates &msg)
{
	m_button_states = msg;

	m_app_web_socket.Send(new_message<msgs::ButtonStates>(msg));
}

void ApplicationService::HandleAppRxButtonColors(const msgs::ButtonColors &msg)
{
	m_button_colors = msg;

	m_buttons_web_socket.Send(new_message<msgs::ButtonColors>(msg));
}

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
