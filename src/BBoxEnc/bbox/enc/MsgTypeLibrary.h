/**
 * @file
 *
 * Header file for the bbox::enc::MsgTypeLibrary class.
 *
 */

#ifndef __BBOX__ENC__MSG_TYPE_LIBRARY_H__
#define __BBOX__ENC__MSG_TYPE_LIBRARY_H__

#include <map>
#include <string>
#include <typeindex>

#include <bbox/enc/MsgType.h>
#include <bbox/enc/MsgPtr.h>

namespace bbox {
namespace enc {

/**
 * A library that stores all the message types.
 */
class MsgTypeLibrary
{
public:

	template <typename Type>
	static MsgType FindByType()
	{
		return FindByType(typeid(Type));
	}

	static MsgType FindByType(const std::type_index &type)
	{
		MsgType result;

		Instance &instance{ g_instance() };

		auto it = instance.by_type.find(type);

		if (it != instance.by_type.end())
			result = it->second;

		return result;
	}

	static MsgType FindByName(const std::string &name)
	{
		MsgType result;

		Instance &instance{ g_instance() };

		auto it = instance.by_name.find(name);

		if (it != instance.by_name.end())
			result = it->second;

		return result;
	}

	template <typename Type>
	class Registration
	{
		Registration() = delete;
		Registration(const Registration &) = delete;
		Registration(Registration &&) = delete;
		Registration &operator =(const Registration &) = delete;
		Registration &operator =(Registration &&) = delete;

	public:
		Registration(std::string &&name, std::function<MsgAnyPtr()> &&constructor)
		{
			g_instance().Register(
				typeid(Type),
				std::move(name),
				std::move(constructor));
		}

		~Registration()
		{
			g_instance().Deregister(typeid(Type));
		}

		void EnsureRegistered()
		{
		}
	};

private:

	struct Instance
	{
		std::map<std::type_index, MsgType> by_type;
		std::map<std::string, MsgType> by_name;

		void Register(std::type_index type, std::string &&name, std::function<MsgAnyPtr()> &&constructor);
		void Deregister(std::type_index type);
	};

	static Instance &g_instance();
};

} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__MSG_TYPE_LIBRARY_H__
