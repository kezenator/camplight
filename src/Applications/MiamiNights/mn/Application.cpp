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

#include <bbox/TextCoding.h>
#include <shellapi.h>


namespace mn {

ApplicationService::ApplicationService(const std::string &name,
                       bbox::rt::Proactor &parent,
                       const bbox::rt::net::TcpEndpoint &http_listen_endpoint)
    : bbox::rt::Service(name, parent)
    , m_http_listen_endpoint(http_listen_endpoint)
    , m_thread_pool("thread-pool", *this)
    , m_console_shutdown_service("console-shutdown-service", *this)
    , m_network_change_service("network-change-service", *this)
    , m_ssdp_discovery_service("ssdp-discovery-service", *this)
    , m_ssdp_advert("ssdp-advert", *this, "buttonbox-target.kezenator.com", 60,
        std::bind(&ApplicationService::SsdpAdvertCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    , m_http_server("http-server", *this)
    , m_http_debug_website("http-debug-website", *this, m_http_server)
	//, m_audio_service("audio service", *this)
	, m_buttons_web_socket("buttons-web-socket", *this,
		m_http_server,
		"/ws/buttons",
		"12.09.2018.buttons.miami-nights.kezenator.com",
		std::bind(&ApplicationService::HandleWebSocketStateChanged, this))
	, m_app_web_socket("app-web-socket", *this,
		m_http_server,
		"/ws/app",
		"12.09.2018.app.miami-nights.kezenator.com",
		std::bind(&ApplicationService::HandleWebSocketStateChanged, this))
	, m_emulator_runner("emulator-runner", *this,
		std::bind(&ApplicationService::HandleEmulatorCompleted, this))
	, m_emulator_joystick("emulator-joystick", *this)
{
	SetThisDependantOn(m_http_server);

	m_buttons_web_socket.RegisterHandler(&ApplicationService::HandleButtonRxButtonState, this);
	m_app_web_socket.RegisterHandler(&ApplicationService::HandleAppRxButtonColors, this);
	m_app_web_socket.RegisterHandler(&ApplicationService::HandleAppRxStartEmulator, this);
}

void ApplicationService::HandleStarting()
{
    m_http_server.AddServer(m_http_listen_endpoint,
        std::bind(&ApplicationService::HttpRequestHandler, this, std::placeholders::_1));

    std::string url = bbox::Format("http://127.0.0.1:%d/index.html?safe", m_http_listen_endpoint.GetPort());

    std::string application = "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";

    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));

    ::CreateProcessW(
        bbox::TextCoding::Win32_UTF8_to_UTF16(application).c_str(),
        const_cast<wchar_t *>(bbox::TextCoding::Win32_UTF8_to_UTF16(bbox::Format(
            "\"%s\" --kiosk --disable-session-crashed-bubble --incognito --disable-infobars %s",
            application,
            url)).c_str()),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi);

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

void ApplicationService::SsdpAdvertCallback(const bbox::rt::net::IpAddress &interface_addr, bool &out_should_send, std::string &out_location)
{
    if (m_http_listen_endpoint.GetAddress().is_unspecified()
        || (m_http_listen_endpoint.GetAddress() == interface_addr))
    {
        out_should_send = true;
        out_location = bbox::Format(
            "ws://%s/ws/buttons",
            bbox::rt::net::TcpEndpoint(interface_addr, m_http_listen_endpoint.GetPort()).ToString());
    }
}

void ApplicationService::HttpRequestHandler(bbox::http::Request &request)
{
    if (request.RespondWithResource(g_resource_files()))
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

void ApplicationService::HandleWebSocketStateChanged()
{
	if (m_app_web_socket.IsOpen()
		&& m_buttons_web_socket.IsOpen())
	{
		m_app_web_socket.Send(new_message<msgs::RetransmitRequired>());
		m_buttons_web_socket.Send(new_message<msgs::RetransmitRequired>());
	}
}

void ApplicationService::HandleButtonRxButtonState(const msgs::ButtonStates &msg)
{
	m_button_states = msg;

	// Send to the emulator if running,
	// else to the WebSite

	if (m_emulator_runner.IsRunning())
	{
		m_emulator_joystick.SetStates(msg);
	}
	else
	{
		m_app_web_socket.Send(new_message<msgs::ButtonStates>(msg));
	}
}

void ApplicationService::HandleAppRxButtonColors(const msgs::ButtonColors &msg)
{
	m_button_colors = msg;

	m_buttons_web_socket.Send(new_message<msgs::ButtonColors>(msg));
}

void ApplicationService::HandleAppRxStartEmulator(const msgs::StartEmulator &msg)
{
	// Return default button states to the Website,
	// start the emulator,
	// and send it the current button states

	m_app_web_socket.Send(new_message<msgs::ButtonStates>(DefaultStates()));

	m_emulator_runner.Start(msg.game);
    m_emulator_joystick.SetEmulatorRunning(true);
	m_emulator_joystick.SetStates(m_button_states);
}

void ApplicationService::HandleEmulatorCompleted()
{
	// Disable the joystick,
	// tell the Website we're finished,
	// and forward it the current button states

    m_emulator_joystick.SetEmulatorRunning(false);
    m_emulator_joystick.SetStates(DefaultStates());

	m_app_web_socket.Send(new_message<msgs::EmulatorCompleted>());
	m_app_web_socket.Send(new_message<msgs::ButtonStates>(m_button_states));
}

msgs::ButtonStates ApplicationService::DefaultStates()
{
	msgs::ButtonStates result;

	result.back_state = false;
	result.play_state = false;
	result.button_states = { false, false, false, false, false, false };

	return result;
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
    else if ((argc >= 3)
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
