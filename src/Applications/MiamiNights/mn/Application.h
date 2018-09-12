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
#include <bbox/http/server/HttpServer.h>
#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/audio/AudioService.h>

#include <mn/MessageWebSocket.h>

#include <mn/msgs/ButtonStates.h>
#include <mn/msgs/ButtonColors.h>

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

	void HttpRequestHandler(bbox::http::Request &request);

	void HandleButtonRxButtonState(const msgs::ButtonStates &msg);
	void HandleAppRxButtonColors(const msgs::ButtonColors &msg);

	bbox::rt::net::TcpEndpoint m_http_listen_endpoint;
	bbox::rt::ConsoleShutdownService m_console_shutdown_service;
	bbox::http::server::HttpServer m_http_server;
	bbox::http::debug::HttpDebugWebsite m_http_debug_website;
	bbox::audio::AudioService m_audio_service;

	mn::MessageWebSocket m_buttons_web_socket;
	mn::MessageWebSocket m_app_web_socket;

	msgs::ButtonStates m_button_states;
	msgs::ButtonColors m_button_colors;

}; // class ApplicationService

} // namespace camplight

#endif // __MN__APPLICATION_H__
