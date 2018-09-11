/**
 * @file
 *
 * Header for the bbox::http::Request class.
 */

#ifndef __BBOX__HTTP__REQUEST_H__
#define __BBOX__HTTP__REQUEST_H__

#include <bbox/rt/net/TcpEndpoint.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace bbox {
namespace http {

// Forward declarations
class ResourceFileSet;
class Response;
namespace server {
class HttpServer;
namespace details {
class Connection;
class WebSocketConnection;
}
}

/**
 * Value that represents a handle to a response.
 */
class Request
{
	friend class Response;
	friend class server::details::Connection;
	friend class server::details::WebSocketConnection;

private:

	using RequestType = boost::beast::http::request<boost::beast::http::string_body>;
	using RequestPtr = std::shared_ptr<RequestType>;

	class Pimpl
	{
	public:
		Pimpl(server::HttpServer &server, server::details::Connection *connection_ptr, RequestPtr &&request_ptr);
		~Pimpl();

		void SetHandled();
		bool NotHandled() { return !m_handled; }

		server::HttpServer &m_server;
		server::details::Connection *m_connection_ptr;
		RequestPtr m_request_ptr;

	private:
		bool m_handled;
	};

	using PimplPtr = std::shared_ptr<Pimpl>;

	Request(server::HttpServer &server, server::details::Connection *connection_ptr, RequestPtr &&request_ptr);

public:

	using Method = boost::beast::http::verb;

	Request() = default;
	Request(const Request &other) = default;
	Request(Request &&other) = default;
	~Request() = default;

	Request &operator =(const Request &other) = default;
	Request &operator =(Request &&other) = default;

	explicit operator bool() const
	{
		return m_pimpl_ptr.operator bool();
	}

	bool operator !() const
	{
		return !m_pimpl_ptr;
	}

	bbox::rt::net::TcpEndpoint GetLocalEndpoint() const;
	bbox::rt::net::TcpEndpoint GetRemoteEndpoint() const;

	std::string GetHost() const;
	std::string GetRootUrl() const;

	std::string GetResource() const;

	bool HasHeader(const std::string &str);
	std::string GetHeader(const std::string &header);

	bool HasQuery(const std::string &param);
	std::string GetQuery(const std::string &param);
	std::string GetFullQueryString();

	Method GetMethod();
	std::string GetMethodString();

	bool CheckIsWebSocketUpgradeOrRespondWithError(const std::string &protocol);

	std::string GetContent();

	bool RespondWithResource(const ResourceFileSet &resources);
	bool RespondWithResource(const ResourceFileSet &resources, const std::string &resource_path);

	void RespondWithResourceOrNotFoundError(const ResourceFileSet &resources);
	void RespondWithResourceOrNotFoundError(const ResourceFileSet &resources, const std::string &resource_path);

	void RespondWithNotFoundError();
	void RespondWithBadRequestError(const std::string &error);

	void RespondWithTemporaryRedirect(const std::string &location);

	void RespondWithServerError(const std::string &message);
	void RespondWithMethodNotAllowedError(const std::string &allowed_methods);

private:
	PimplPtr m_pimpl_ptr;
};

} // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__REQUEST_H__