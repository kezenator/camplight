/**
 * @file
 *
 * Implementation file for the bbox::http::debug::HttpDebugWebsite class.
 */

#include <bbox/http/debug/HttpDebugWebsite.h>
#include <bbox/http/debug/Resources.h>

#include <bbox/debug/DebugQuery.h>
#include <bbox/debug/DebugReport.h>

#include <bbox/http/server/HttpServer.h>

#include <bbox/enc/FromXml.h>
#include <bbox/enc/ToXml.h>
#include <bbox/enc/ToDebugString.h>

#include <bbox/http/debug/msgs/DebugReportNotification.h>
#include <bbox/http/debug/msgs/QueryResponse.h>

#include <boost/algorithm/string/replace.hpp>

namespace bbox {
namespace http {
namespace debug {

HttpDebugWebsite::HttpDebugWebsite(const std::string &name, bbox::rt::Service &parent, bbox::http::server::HttpServer &server)
	: bbox::rt::Service(name, parent)
	, m_reources_request_handler("resource-request-handler", *this, server)
	, m_websocket_request_handler("websocket-request-handler", *this, server)
	, m_web_socket()
	, m_dispatcher()
	, m_output_requested(false)
	, m_requested_debug_enables()
	, m_active_debug_enables()
{
	SetThisDependantOn(server);

	m_dispatcher.Register(&HttpDebugWebsite::HandleWebSocketMessageEnableRequest, this);
	m_dispatcher.Register(&HttpDebugWebsite::HandleWebSocketMessageQueryRequest, this);
}

HttpDebugWebsite::~HttpDebugWebsite()
{
}

void HttpDebugWebsite::HandleStarting()
{
	m_reources_request_handler.Login("/debug", std::bind(&HttpDebugWebsite::HandleResourceRequest, this, std::placeholders::_1));
	m_websocket_request_handler.Login("/debug/ws", std::bind(&HttpDebugWebsite::HandleWebSocketRequest, this, std::placeholders::_1));

	NotifyStarted();
}

void HttpDebugWebsite::HandleStopping()
{
	m_web_socket = server::ServerWebSocket();

	m_reources_request_handler.Logout();
	m_websocket_request_handler.Logout();

	RequestStopAllChildren();
	NotifyStopped();
}

void HttpDebugWebsite::PrintState(bbox::DebugOutput &out) const
{
	out.Format("Location: ");
	{
		bbox::DebugOutput link_out(BBOX_FUNC, out, bbox::DebugOutput::Mime_Text_Url);
		link_out.Format("/debug");
	}
	out << std::endl;

	out.Format("Debug Requested: %s\n", m_output_requested);
	out.Format("Debug Enables: %s\n", bbox::enc::ToDebugString(m_active_debug_enables));
}

void HttpDebugWebsite::OutputToTarget(bbox::debug::DebugReport report)
{
	// We only handle debug while in the running state

	if (GetLocalRunLevel() == bbox::rt::RunLevel::RUNNING)
	{
		if (m_output_requested
			&& m_web_socket.IsOpen())
		{
			bbox::enc::MsgPtr<msgs::DebugReportNotification> msg = new_message<msgs::DebugReportNotification>();
			ConvertDebugReport(report, *msg);

			WebSocketSend(msg);
		}
	}
}

void HttpDebugWebsite::HandleResourceRequest(http::Request &request)
{
	std::string resource = request.GetResource();

	if (resource == "/debug")
	{
		request.RespondWithTemporaryRedirect("/debug/");
	}
	else if ((resource == "/debug/")
		|| (resource.substr(0, 13) == "/debug/state/"))
	{
		request.RespondWithResourceOrNotFoundError(g_resource_files, "/debug/index.html");
	}
	else
	{
		request.RespondWithResourceOrNotFoundError(g_resource_files);
	}
}

void HttpDebugWebsite::HandleWebSocketRequest(http::Request &request)
{
	if (!request.CheckIsWebSocketUpgradeOrRespondWithError("11.09.2018.debug.http.bbox.kezenator.com"))
		return;

	if (m_web_socket.IsOpen())
	{
		m_web_socket.Close();
		HandleWebSocketState(std::errc::connection_aborted);
	}
	m_web_socket = server::ServerWebSocket();

	m_web_socket = server::ServerWebSocket::Upgrade(
		request,
		"11.09.2018.debug.http.bbox.kezenator.com",
		std::bind(&HttpDebugWebsite::HandleWebSocketState, this, std::placeholders::_1),
		std::bind(&HttpDebugWebsite::HandleWebSocketMessage, this, std::placeholders::_1));
}

void HttpDebugWebsite::HandleWebSocketState(const bbox::Error &error)
{
	m_output_requested = false;
	m_requested_debug_enables.clear();

	UpdateOverallDebugEnables();
}

void HttpDebugWebsite::HandleWebSocketMessage(const std::string &msg_str)
{
	bbox::enc::FromXml from_xml(msg_str);

	bbox::enc::MsgAnyPtr msg;

	from_xml.DecodeNamedValue("message", msg);
	from_xml.EnsureAllDecoded();

	if (!msg)
	{
		from_xml.SetError("Received nullptr message");
	}

	if (!from_xml.HasError())
	{
		if (!m_dispatcher.Dispatch(msg))
			from_xml.SetError(bbox::Format("No handler or handler error for message of type %s", msg.GetType().GetName()));
	}

	if (from_xml.HasError())
	{
		bbox::DebugOutput out(BBOX_FUNC, bbox::DebugOutput::Error);
		if (out)
		{
			out.Format("HTTP Debug: Error decoding received message: %s\n", from_xml.GetErrorString());
		}

		m_web_socket.Close();
		return;
	}
}

bool HttpDebugWebsite::HandleWebSocketMessageQueryRequest(const msgs::QueryRequest &msg)
{
	std::vector<bbox::debug::DebugQueryResult> query_results =
		bbox::debug::DebugQuery::DoQuery(msg.query);

	auto response_msg = new_message<msgs::QueryResponse>();

	auto &response = *response_msg;

	response.request_id = msg.request_id;
	response.entries.clear();
	response.entries.reserve(query_results.size());

	for (const bbox::debug::DebugQueryResult &entry : query_results)
	{
		response.entries.emplace_back();

		msgs::QueryResponseEntry &response_entry = response.entries.back();

		response_entry.path = entry.full_path;
		response_entry.parent_path = entry.parent_path;
		for (const bbox::debug::DebugChildEntry &child : entry.children)
		{
			response_entry.children.emplace_back();

			msgs::QueryResponseChild &response_child = response_entry.children.back();
			response_child.name = child.name;
			response_child.path = child.path;
			response_child.short_description = child.short_description;
		}

		ConvertDebugReport(entry.report, response_entry.report);
	}

	WebSocketSend(response_msg);

	return true;
}

bool HttpDebugWebsite::HandleWebSocketMessageEnableRequest(const msgs::EnableRequest &msg)
{
	m_output_requested = msg.enabled;

	m_requested_debug_enables.clear();
	for (const auto &entry : msg.debug_enables)
		m_requested_debug_enables.insert(entry);

	UpdateOverallDebugEnables();

	return true;
}

void HttpDebugWebsite::WebSocketSend(const bbox::enc::MsgAnyPtr &msg)
{
	if (m_web_socket.IsOpen())
	{
		m_web_socket.Send(bbox::enc::ToXml::ConvertToString("message", msg));
	}
}

void HttpDebugWebsite::UpdateOverallDebugEnables()
{
	if (m_requested_debug_enables!= m_active_debug_enables)
	{
		m_active_debug_enables = m_requested_debug_enables;

		UpdateDebugEnables(std::set<std::string>(m_active_debug_enables));
	}
}

void HttpDebugWebsite::ConvertDebugReport(const bbox::debug::DebugReport &src, msgs::DebugReportNotification &dest)
{
	std::stringstream stream;

	// Print out the report data in the correct format

	const std::string &data = src.Data();
	const std::vector<bbox::debug::DebugReport::OutputEntry> &output_entries = src.OutputEntries();
	const std::vector<bbox::debug::DebugReport::SpanEntry> &span_entries = src.SpanEntries();

	for (const bbox::debug::DebugReport::SpanEntry &span : span_entries)
	{
		const bbox::debug::DebugReport::OutputEntry &output = output_entries[span.output_index];

		switch (output.mime_type)
		{
		case DebugOutput::Mime_Text_Plain:
			stream << HtmlEscape(data.substr(span.data_start_index, span.data_length));
			break;

		case DebugOutput::Mime_Text_Html:
			stream.write(data.c_str() + span.data_start_index, span.data_length);
			break;

		case DebugOutput::Mime_Text_Url:
		{
			std::string quoted_url = HtmlEscape(data.substr(span.data_start_index, span.data_length));
			stream << "<a href='" << quoted_url << "'>" << quoted_url << "</a>";
		}
		break;

		case DebugOutput::Mime_Text_Debug_Full_Path:
		{
			std::string path_html = HtmlEscape(data.substr(span.data_start_index, span.data_length));
			std::string path_uri = UriEscapeExceptForwardSlash(data.substr(span.data_start_index, span.data_length));
			stream << "<a href='/debug/state" << path_uri <<
				"' onclick='return app.pushState_QueryState(\"" << path_html << "\");'>"
				<< path_html << "</a>";
		}
		break;
		}
	}

	dest.contents_html = stream.str();
}

std::string HttpDebugWebsite::HtmlEscape(std::string &&str)
{
	std::string result(std::move(str));
	boost::replace_all(result, "&", "&amp;");
	boost::replace_all(result, "<", "&lt;");
	boost::replace_all(result, ">", "&gt;");
	boost::replace_all(result, "\"", "&quot;");
	boost::replace_all(result, "\'", "&apos;");
	return result;
}

std::string HttpDebugWebsite::UriEscapeExceptForwardSlash(std::string &&str)
{
	static const char constexpr lookup[] = "0123456789ABCDEF";

	std::stringstream stream;

	for (char ch : str)
	{
		if (std::isalnum(ch)
			|| (ch == '-')
			|| (ch == '_')
			|| (ch == '.')
			|| (ch == '~')
			|| (ch == '/'))
		{
			stream << ch;
		}
		else
		{
			stream << '%'
				<< lookup[(ch >> 4) & 0x0F]
				<< lookup[ch & 0x0F];
		}
	}

	return stream.str();
}

} // namespace bbox::http::debug
} // namespace bbox::http
} // namespace bbox
