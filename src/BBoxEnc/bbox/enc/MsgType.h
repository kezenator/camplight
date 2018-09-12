/**
 * @file
 *
 * Header file for the bbox::enc::MsgType class.
 *
 */

#ifndef __BBOX__ENC__MSG_TYPE_H__
#define __BBOX__ENC__MSG_TYPE_H__

#include <functional>
#include <memory>
#include <string>
#include <typeindex>

namespace bbox {
namespace enc {

// Forward declarations
class MsgTypeLibrary;
class MsgAnyPtr;

/**
 * The type of a message.
 */
class MsgType
{
	friend class MsgTypeLibrary;

public:
	MsgType() = default;
	~MsgType() = default;
	MsgType(const MsgType &) = default;
	MsgType(MsgType &&) = default;
	MsgType &operator =(const MsgType &) = default;
	MsgType &operator =(MsgType &&) = default;

	explicit operator bool() const
	{
		return m_ptr.operator bool();
	}

	bool operator ==(const MsgType &other) const
	{
		if (!m_ptr
			&& !other.m_ptr)
		{
			// Both null
			return true;
		}

		if (!m_ptr
			|| !other.m_ptr)
		{
			// Only one is null
			return false;
		}

		// Both not null

		return (m_ptr->type == other.m_ptr->type);
	}

	bool operator !=(const MsgType &other) const
	{
		return !operator ==(other);
	}

	bool operator <(const MsgType &other) const
	{
		if (!m_ptr
			&& !other.m_ptr)
		{
			// Both null
			return false;
		}

		if (!m_ptr
			&& other.m_ptr)
		{
			return true;
		}

		if (m_ptr
			&& !other.m_ptr)
		{
			return false;
		}

		// Both not null

		return m_ptr->type < other.m_ptr->type;
	}

	const std::string &GetName() const
	{
		return m_ptr->name;
	}

	const std::type_index &GetType() const
	{
		return m_ptr->type;
	}

	MsgAnyPtr CreateNewObject() const;

private:

	struct Pimpl
	{
		std::type_index &type;
		std::string name;
		std::function<MsgAnyPtr()> constructor;

		Pimpl(std::type_index &_type, std::string &&_name, std::function<MsgAnyPtr()> &&_constructor)
			: type(_type)
			, name(std::move(_name))
			, constructor(std::move(_constructor))
		{
		}
	};

	MsgType(std::type_index &_type, std::string &&_name, std::function<MsgAnyPtr()> &&_constructor)
		: m_ptr(std::make_shared<Pimpl>(_type, std::move(_name), std::move(_constructor)))
	{
	}

	std::shared_ptr<Pimpl> m_ptr;
};

} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__MSG_TYPE_H__
