/**
 * @file
 *
 * Header file for the bbox::enc::MsgAnyPtr class.
 *
 */

#ifndef __BBOX__ENC__MSG_ANY_PTR_H__
#define __BBOX__ENC__MSG_ANY_PTR_H__

#include <bbox/enc/MsgType.h>

namespace bbox {
namespace enc {

// Forward declarations
class FromBinary;
class FromTextFormat;
class ToBinary;
class ToTextFormat;
template <typename Type>
class MsgPtr;

/**
 * Holds a pointer to an object of any MsgType type.
 */
class MsgAnyPtr
{
	template <typename Type>
	friend class MsgPtr;

	friend class FromBinary;
	friend class FromTextFormat;
	friend class ToBinary;
	friend class ToTextFormat;

public:
	MsgAnyPtr() = default;
	~MsgAnyPtr() = default;
	MsgAnyPtr(const MsgAnyPtr &) = default;
	MsgAnyPtr(MsgAnyPtr &&) = default;
	MsgAnyPtr &operator =(const MsgAnyPtr &) = default;
	MsgAnyPtr &operator =(MsgAnyPtr &&) = default;

	explicit operator bool() const
	{
		return m_value.operator bool();
	}

	bool operator !() const
	{
		return !m_value.operator bool();
	}

	MsgType GetType() const
	{
		MsgType result;

		if (m_value)
			result = m_value->type;

		return result;
	}

private:
	struct Value
	{
		explicit Value(const MsgType &_type)
			: type(_type)
		{
		}

		virtual ~Value()
		{
		}

		virtual void DecodeContents(FromBinary &m) = 0;
		virtual void DecodeContents(FromTextFormat &m) = 0;
		virtual void EncodeContents(ToBinary &m) = 0;
		virtual void EncodeContents(ToTextFormat &m) = 0;

		MsgType type;
	};

	explicit MsgAnyPtr(std::shared_ptr<Value> &&value)
		: m_value(std::move(value))
	{
	}

	std::shared_ptr<Value> m_value;
};

} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__MSG_ANY_PTR_H__
