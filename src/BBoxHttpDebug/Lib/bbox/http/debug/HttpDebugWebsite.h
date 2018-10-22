/**
 * @file
 *
 * Header file for the bbox::http::debug::HttpDebugWebsite class.
 */

#include <bbox/rt/Service.h>
#include <bbox/rt/Timer.h>
#include <bbox/http/server/RequestHandler.h>
#include <bbox/http/server/ServerWebSocket.h>
#include <bbox/debug/DebugTarget.h>
#include <bbox/enc/MsgAnyPtr.h>
#include <bbox/enc/Dispatcher.h>

#include <bbox/http/debug/msgs/QueryRequest.h>
#include <bbox/http/debug/msgs/EnableRequest.h>

namespace bbox {
namespace http {
namespace debug {

// Forward declarations
namespace msgs {
class DebugReportNotification;
}

class HttpDebugWebsite : public bbox::rt::Service, private bbox::debug::DebugTarget
{
public:
	HttpDebugWebsite(const std::string &name, bbox::rt::Service &parent, bbox::http::server::HttpServer &server);
	~HttpDebugWebsite();

private:
	// Implements bbox::rt::Service
	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	// Implements bbox::debug::DebugTarget
	void OutputToTarget(bbox::debug::DebugReport report) override;

	void HandleResourceRequest(http::Request &request);
	void HandleWebSocketRequest(http::Request &request);

	void HandleWebSocketState(const bbox::Error &error);
	void HandleWebSocketMessage(const std::string &msg_str);

	bool HandleWebSocketMessageQueryRequest(const msgs::QueryRequest &msg);
	bool HandleWebSocketMessageEnableRequest(const msgs::EnableRequest &msg);

	void WebSocketSend(const bbox::enc::MsgAnyPtr &msg);
	void UpdateOverallDebugEnables();

	static void ConvertDebugReport(const bbox::debug::DebugReport &src, msgs::DebugReportNotification &dest);
	static std::string HtmlEscape(std::string &&str);
	static std::string UriEscapeExceptForwardSlash(std::string &&str);

	server::RequestHandler m_reources_request_handler;
	server::RequestHandler m_websocket_request_handler;

	server::ServerWebSocket m_web_socket;

	bbox::enc::Dispatcher m_dispatcher;

	bool m_output_requested;
	std::set<std::string> m_requested_debug_enables;
	std::set<std::string> m_active_debug_enables;
};

} // namespace bbox::http::debug
} // namespace bbox::http
} // namespace bbox
