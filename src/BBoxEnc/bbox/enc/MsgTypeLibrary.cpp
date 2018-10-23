/**
 * @file
 *
 * Implementation file for the bbox::enc::MsgTypeLibrary class.
 *
 */

#include <bbox/enc/MsgPtr.h>

#include <iostream>
#include <cassert>

namespace bbox {
namespace enc {

MsgTypeLibrary::Instance &MsgTypeLibrary::g_instance()
{
	static MsgTypeLibrary::Instance instance;
	return instance;
}

void MsgTypeLibrary::Instance::Register(std::type_index type, std::string &&name, std::function<MsgAnyPtr()> &&constructor)
{
	if ((by_type.count(type) != 0)
		|| (by_name.count(name) != 0))
	{
		std::cerr << "Duplicate MsgType Registration: " << name << std::endl;
		abort();
	}

	MsgType msg_type{ type, std::move(name), std::move(constructor) };

	by_type[msg_type.GetType()] = msg_type;
	by_name[msg_type.GetName()] = msg_type;
}

void MsgTypeLibrary::Instance::Deregister(std::type_index type)
{
	auto it = by_type.find(type);
	assert(it != by_type.end());

	MsgType msg_type{ it->second };

	by_type.erase(it);
	by_name.erase(msg_type.GetName());
}

} // namespace bbox::enc
} // namespace bbox
