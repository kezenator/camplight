/**
 * @file
 *
 * Header file for the mn::MessageWebSocket class.
 */

#ifndef __MN__MESSAGE_WEB_SOCKET_H__
#define __MN__MESSAGE_WEB_SOCKET_H__

#include <bbox/rt/Service.h>
#include <bbox/http/server/ServerWebSocket.h>
#include <bbox/http/server/RequestHandler.h>

#include <bbox/enc/MsgAnyPtr.h>

namespace mn {

/**
 * A web-socket that can send and receive messages to a single remote party.
 */
class MessageWebSocket: public bbox::rt::Service
{
public:
	MessageWebSocket(
		const std::string &name,
		bbox::rt::Service &parent,
	    bbox::http::server::HttpServer &server,
		std::string &&path,
		std::string &&protocol);
	~MessageWebSocket();

	template <typename HandlerType, typename MsgType>
	void RegisterHandler(void (HandlerType::*method)(const MsgType &), HandlerType *handler)
	{
		m_msg_handlers[bbox::enc::MsgTypeLibrary::FindByType<MsgType>()] =
			[=](const bbox::enc::MsgAnyPtr &msg) -> void
			{
			    (handler->*method)(*message_cast<MsgType>(msg));
			};
	}

	bool IsOpen()
	{
		return m_socket.IsOpen();
	}

	void Send(const bbox::enc::MsgAnyPtr &msg);

private:
	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	void HandleHttpRequest(bbox::http::Request request);
	void HandleWebSocketState(bbox::Error error);
	void HandleWebSocketRxMessage(const std::string &str);

	bbox::http::server::HttpServer &m_server;
	const std::string m_path;
	const std::string m_protocol;
	bbox::http::server::RequestHandler m_request_handler;
	bbox::http::server::ServerWebSocket m_socket;
	bbox::rt::DebugEnable m_debug_enable;

	std::map<bbox::enc::MsgType, std::function<void(const bbox::enc::MsgAnyPtr &)>> m_msg_handlers;
};

} // namespace mn

#endif // __MN__MESSAGE_WEB_SOCKET_H__
