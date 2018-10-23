/**
 * @file
 *
 * Header file for the bbox::enc::MsgPtr class.
 *
 */

#ifndef __BBOX__ENC__MSG_PTR_H__
#define __BBOX__ENC__MSG_PTR_H__

#include <bbox/enc/MsgAnyPtr.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace bbox {
namespace enc {

/**
 * Holds a pointer to an object of any MsgType type.
 */
template <typename Type>
class MsgPtr
{
public:
	MsgPtr() = default;
	~MsgPtr() = default;
	MsgPtr(const MsgPtr &) = default;
	MsgPtr(MsgPtr &&) = default;
	MsgPtr &operator =(const MsgPtr &) = default;
	MsgPtr &operator =(MsgPtr &&) = default;

	explicit operator bool() const
	{
		return m_value.operator bool();
	}

	bool operator !() const
	{
		return !m_value.operator bool();
	}

	operator MsgAnyPtr() const
	{
		return MsgAnyPtr(m_value);
	}

	const Type &operator *() const
	{
		return m_value->value;
	}

	Type &operator *()
	{
		return m_value->value;
	}

	const Type *operator ->() const
	{
		return &m_value->value;
	}

	Type *operator ->()
	{
		return &m_value->value;
	}

	MsgType GetType() const
	{
		MsgType result;

		if (m_value)
			result = m_value->type;

		return result;
	}

	template <typename ... Args>
	static MsgPtr CreateNewObject(Args &&...args)
	{
		return MsgPtr(std::make_shared<Value>(std::move(args)...));
	}

	static MsgPtr TryCast(const MsgAnyPtr &other)
	{
		MsgPtr result;

		if (other)
		{
			MsgType type = MsgTypeLibrary::FindByType(typeid(Type));
			if (type)
			{
				if (type == other.m_value->type)
				{
					result = MsgPtr(std::dynamic_pointer_cast<Value>(other.m_value));
				}
			}
		}

		return result;
	}

private:

	struct Value : public MsgAnyPtr::Value
	{
		template <typename ... Args>
		Value(Args && ...args)
			: MsgAnyPtr::Value(MsgTypeLibrary::FindByType<Type>())
			, value(std::move(args)...)
		{
		}

		virtual ~Value()
		{
		}

		void DecodeContents(FromBinary &m) override
		{
			m.Read(value);
		}

		void DecodeContents(FromTextFormat &m) override
		{
			m.DecodeValue(value);
		}

		void EncodeContents(ToBinary &m) override
		{
			m.Write(value);
		}

		void EncodeContents(ToTextFormat &m) override
		{
			m.SetValue(value);
		}

		Type value;
	};

	explicit MsgPtr(std::shared_ptr<Value> &&value)
		: m_value(std::move(value))
	{
	}

	std::shared_ptr<Value> m_value;
};

} // namespace bbox::enc
} // namespace bbox

template <typename Type, typename ... Args>
bbox::enc::MsgPtr<Type> new_message(Args && ...args)
{
	return bbox::enc::MsgPtr<Type>::CreateNewObject(std::move(args)...);
}

template <typename Type>
bbox::enc::MsgPtr<Type> message_cast(const bbox::enc::MsgAnyPtr &val)
{
	return bbox::enc::MsgPtr<Type>::TryCast(val);
}

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

#include <bbox/enc/details/FromBinaryAction.h>
#include <bbox/enc/details/FromTextFormatAction.h>
#include <bbox/enc/details/ToBinaryAction.h>
#include <bbox/enc/details/ToTextFormatAction.h>

#endif // __BBOX__ENC__MSG_PTR_H__
