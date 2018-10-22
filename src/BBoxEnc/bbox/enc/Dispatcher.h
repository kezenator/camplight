/**
 * @file
 *
 * Header file for the bbox::enc::Dispatcher class.
 *
 */

#ifndef __BBOX__ENC__DISPATCHER_H__
#define __BBOX__ENC__DISPATCHER_H__

#include <functional>
#include <map>

#include <bbox/enc/MsgAnyPtr.h>
#include <bbox/enc/MsgPtr.h>
#include <bbox/enc/MsgTypeLibrary.h>

namespace bbox {
namespace enc {

/**
 * A class that maintains a set of message handlers and can
 * dispatch messages to the correct handler.
 */
class Dispatcher
{
public:

	Dispatcher() = default;
	~Dispatcher() = default;
	Dispatcher(const Dispatcher &) = default;
	Dispatcher(Dispatcher &&) = default;
	Dispatcher &operator =(const Dispatcher &) = default;
	Dispatcher &operator =(Dispatcher &&) = default;

	bool Dispatch(const MsgAnyPtr &msg)
	{
		auto it = m_handlers.find(msg.GetType());

		if (it == m_handlers.end())
			return false;

		return it->second(msg);
	}

	template <typename MsgType>
	void Register(std::function<bool(const MsgType &)> handler)
	{
		MsgType::g_msg_registration.EnsureRegistered();

		m_handlers[MsgTypeLibrary::FindByType<MsgType>()] = [handler](const MsgAnyPtr &msg)
		{
			MsgPtr<MsgType> cast_msg = message_cast<MsgType>(msg);
			if (!cast_msg)
				return false;

			return handler(*cast_msg);
		};
	}

	template <typename MsgType, typename HandlerType>
	void Register(bool (HandlerType::*method)(const MsgType &), HandlerType *handler)
	{
		MsgType::g_msg_registration.EnsureRegistered();

		m_handlers[MsgTypeLibrary::FindByType<MsgType>()] = [method, handler](const MsgAnyPtr &msg)
		{
			MsgPtr<MsgType> cast_msg = message_cast<MsgType>(msg);
			if (!cast_msg)
				return false;

			return (handler->*method)(*cast_msg);
		};
	}
private:
	std::map<MsgType, std::function<bool(const MsgAnyPtr&)>> m_handlers;
};

} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DISPATCHER_H__
