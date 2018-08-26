/**
 * @file
 *
 * Header file for the bbox::enc::compiler::TypeNameList class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_NAME_LIST_H__
#define __BBOX__ENC__COMPILER__TYPE_NAME_LIST_H__

#include <vector>

#include <bbox/enc/compiler/Token.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that represents a list of type name components - e.g. "std::vector".
 */
class TypeNameList
{
public:
	TypeNameList() = default;
	~TypeNameList() = default;
	TypeNameList(const TypeNameList &) = default;
	TypeNameList(TypeNameList &&) = default;
	TypeNameList &operator =(const TypeNameList &) = default;
	TypeNameList &operator =(TypeNameList &&) = default;

	explicit TypeNameList(std::vector<Token> &&tokens)
		: m_tokens(std::move(tokens))
	{
	}

	const Token &GetStartToken() const
	{
		return m_tokens.front();
	}

	std::string ToString() const
	{
		std::string result;
		bool first = true;

		for (const Token &token : m_tokens)
		{
			if (first)
				first = false;
			else
				result.append("::");
			result.append(token.GetContents());
		}

		return result;
	}

private:
	std::vector<Token> m_tokens;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_NAME_LIST_H__
