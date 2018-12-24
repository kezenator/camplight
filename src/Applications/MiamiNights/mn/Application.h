/**
 * @file
 *
 * Header file for the mn::Application class.
 *
 */

#ifndef __MN__APPLICATION_H__
#define __MN__APPLICATION_H__

#include <bbox/MainWrapper.h>
#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/CoutDebugTarget.h>
#include <bbox/rt/Proactor.h>
#include <bbox/rt/Service.h>
#include <bbox/rt/ConsoleShutdownService.h>
#include <bbox/rt/net/NetworkChangeService.h>
#include <bbox/rt/net/ssdp/SsdpDiscoveryService.h>
#include <bbox/rt/net/ssdp/SsdpAdvert.h>
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/debug/HttpDebugWebsite.h>
//#include <bbox/audio/AudioService.h>

#include <mn/MessageWebSocket.h>
#include <mn/EmulatorRunner.h>
#include <mn/EmulatorJoystick.h>

#include <mn/msgs/ButtonStates.h>
#include <mn/msgs/ButtonColors.h>
#include <mn/msgs/EmulatorCompleted.h>
#include <mn/msgs/RetransmitRequired.h>
#include <mn/msgs/StartEmulator.h>

namespace mn {

class ApplicationService : public bbox::rt::Service
{
public:

	ApplicationService(const std::string &name,
		bbox::rt::Proactor &parent,
		const bbox::rt::net::TcpEndpoint &http_listen_endpoint);

private:

	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

    void SsdpAdvertCallback(const bbox::rt::net::IpAddress &interface_addr, bool &out_should_send, std::string &out_location);

	void HttpRequestHandler(bbox::http::Request &request);

	void HandleWebSocketStateChanged();
	void HandleButtonRxButtonState(const msgs::ButtonStates &msg);
	void HandleAppRxButtonColors(const msgs::ButtonColors &msg);
	void HandleAppRxStartEmulator(const msgs::StartEmulator &msg);
	void HandleEmulatorCompleted();

	msgs::ButtonStates DefaultStates();

    const bbox::rt::net::TcpEndpoint m_http_listen_endpoint;

    bbox::rt::ThreadPool m_thread_pool;
    bbox::rt::ConsoleShutdownService m_console_shutdown_service;
    bbox::rt::net::NetworkChangeService::Impl m_network_change_service;
    bbox::rt::net::ssdp::SsdpDiscoveryService m_ssdp_discovery_service;
    bbox::rt::net::ssdp::SsdpAdvert m_ssdp_advert;
    bbox::http::server::HttpServer m_http_server;
	bbox::http::debug::HttpDebugWebsite m_http_debug_website;
	//bbox::audio::AudioService m_audio_service;

	mn::MessageWebSocket m_buttons_web_socket;
	mn::MessageWebSocket m_app_web_socket;
	mn::EmulatorRunner m_emulator_runner;
	mn::EmulatorJoystick m_emulator_joystick;

	msgs::ButtonStates m_button_states;
	msgs::ButtonColors m_button_colors;

}; // class ApplicationService

} // namespace camplight

#endif // __MN__APPLICATION_H__
