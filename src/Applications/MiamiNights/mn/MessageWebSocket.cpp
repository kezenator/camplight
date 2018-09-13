/**
 * @file
 *
 * Implementation file for the mn::MessageWebSocket class.
 */

#include <mn/MessageWebSocket.h>

#include <bbox/enc/FromXml.h>
#include <bbox/enc/ToDebugString.h>
#include <bbox/enc/ToXml.h>

#include <bbox/http/server/HttpServer.h>

namespace mn {

MessageWebSocket::MessageWebSocket(
	                  const std::string &name,
	                  bbox::rt::Service &parent,
	                  bbox::http::server::HttpServer &server,
	                  std::string &&path,
	                  std::string &&protocol,
	                  std::function<void()> &&connected_changed_handler)
	: bbox::rt::Service(name, parent)
	, m_server(server)
	, m_path(std::move(path))
	, m_protocol(std::move(protocol))
	, m_connected_changed_handler(std::move(connected_changed_handler))
	, m_request_handler("request-handler", *this, server)
	, m_socket()
	, m_debug_enable("messaging", *this)
	, m_msg_handlers()
{
	SetThisDependantOn(server);
}

MessageWebSocket::~MessageWebSocket()
{
}

void MessageWebSocket::HandleStarting()
{
	m_request_handler.Login(
		std::string(m_path),
		std::bind(&MessageWebSocket::HandleHttpRequest, this, std::placeholders::_1));

	NotifyStarted();
}

void MessageWebSocket::HandleStopping()
{
	m_request_handler.Logout();
	m_socket.Close();

	RequestStopAllChildren();
	NotifyStopped();
}

void MessageWebSocket::PrintState(bbox::DebugOutput &out) const
{
	out.Format("Path:     %s\n", m_path);
	out.Format("Protocol: %s\n", m_protocol);
	out.Format("Open:     %s\n", m_socket.IsOpen());
}

void MessageWebSocket::HandleHttpRequest(bbox::http::Request request)
{
	if (!request.CheckIsWebSocketUpgradeOrRespondWithError(m_protocol))
		return;

	m_socket.Close();
	m_socket = bbox::http::server::ServerWebSocket::Upgrade(
		request,
		m_protocol,
		std::bind(&MessageWebSocket::HandleWebSocketState, this, std::placeholders::_1),
		std::bind(&MessageWebSocket::HandleWebSocketRxMessage, this, std::placeholders::_1));
}

void MessageWebSocket::HandleWebSocketState(bbox::Error error)
{
	bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Activity);
	if (out)
	{
		{
			bbox::DebugOutput path_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Debug_Full_Path);
			path_out << GetResourceFullPath();
		}
		out.Format(": State: %s\n", error.ToString());
	}

	m_connected_changed_handler();
}

void MessageWebSocket::HandleWebSocketRxMessage(const std::string &str)
{
	bbox::enc::MsgAnyPtr msg;
	std::function<void(const bbox::enc::MsgAnyPtr &)> handler;

	bbox::enc::FromXml fromXml(str);

	fromXml.DecodeNamedValue("message", msg);
	fromXml.EnsureAllDecoded();

	if (!msg)
		fromXml.SetError("Decoded null message");
	else
	{
		auto it = m_msg_handlers.find(msg.GetType());

		if (it == m_msg_handlers.end())
			fromXml.SetError(bbox::Format("No handler for message of type %s", msg.GetType().GetName()));
		else
			handler = it->second;
	}

	if (fromXml.HasError())
	{
		m_socket.Close();

		bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Error);
		if (out)
		{
			{
				bbox::DebugOutput path_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Debug_Full_Path);
				path_out << GetResourceFullPath();
			}
			out.Format(": RX: Error: %s\n", fromXml.GetErrorString());
			out.Format("String Data:\n");
			out.IncIndent(4);
			out.Format("%s\n", str);
		}
	}
	else // no error
	{
		bbox::DebugOutput out(BBOX_FUNC, m_debug_enable);
		if (out)
		{
			{
				bbox::DebugOutput path_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Debug_Full_Path);
				path_out << GetResourceFullPath();
			}
			out.Format(": RX: %s\n", bbox::enc::ToDebugString(msg));
		}

		handler(msg);
	}
}

void MessageWebSocket::Send(const bbox::enc::MsgAnyPtr &msg)
{
	if (!m_socket.IsOpen())
		return;

	bbox::DebugOutput out(BBOX_FUNC, m_debug_enable);
	if (out)
	{
		{
			bbox::DebugOutput path_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Debug_Full_Path);
			path_out << GetResourceFullPath();
		}
		out.Format(": TX: %s\n", bbox::enc::ToDebugString(msg));
	}

	m_socket.Send(bbox::enc::ToXml::ConvertToString("message", msg));
}

} // namespace mn
