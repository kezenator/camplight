/**
 * @file
 *
 * Header for the bbox::http::client::WebSocketClient class.
 */

#ifndef __BBOX__HTTP__CLIENT__WEB_SOCKET_CLIENT_H__
#define __BBOX__HTTP__CLIENT__WEB_SOCKET_CLIENT_H__

#include <bbox/rt/Resource.h>
#include <bbox/Error.h>

#include <functional>
#include <memory>

namespace bbox {
namespace http {
namespace client {

/**
 * A client connection to a web-socket server.
 */
class WebSocketClient: public rt::Resource
{
public:

	using StateHandler = std::function<void(const bbox::Error &error)>;
	using ReceiveHandler = std::function<void(const std::string &message)>;

	WebSocketClient(
		const std::string &name,
		rt::Resource &parent,
		StateHandler state_handler,
		ReceiveHandler receive_handler);
	WebSocketClient(
		const std::string &name,
		rt::Service &parent,
		StateHandler state_handler,
		ReceiveHandler receive_handler);
	~WebSocketClient();

	bool IsOpen() const;
	void Open(const std::string &uri, const std::string &protocol);
	void CloseConnection();

	void Send(const std::string &str);

protected:
	void PrintState(DebugOutput &out) const override;
	void HandleStopping() override;

private:
	struct Pimpl;
	std::unique_ptr<Pimpl> m_pimpl;
};

} // namespace bbox::http::client
} // namespace bbox::http
} // namespace bbox

#endif // __BBOX__HTTP__CLIENT__WEB_SOCKET_CLIENT_H__
