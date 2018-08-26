/**
 * @file
 *
 * Header file for the bbox::enc::compiler::TypeName class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_NAME_H__
#define __BBOX__ENC__COMPILER__TYPE_NAME_H__

#include <bbox/enc/compiler/TypeNameList.h>

#include <optional>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that represents a type name - e.g. "std::vector<std::string>".
 */
class TypeName
{
public:
	TypeName() = default;
	~TypeName() = default;
	TypeName(const TypeName &) = default;
	TypeName(TypeName &&) = default;
	TypeName &operator =(const TypeName &) = default;
	TypeName &operator =(TypeName &&) = default;

	explicit TypeName(TypeNameList &&type_name, std::optional<std::vector<TypeNameList>> &&opt_template_params)
		: m_type_name(std::move(type_name))
		, m_opt_template_params(std::move((opt_template_params)))
	{
	}

	const Token &GetStartToken() const
	{
		return m_type_name.GetStartToken();
	}

private:
	TypeNameList m_type_name;
	std::optional<std::vector<TypeNameList>> m_opt_template_params;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_NAME_H__
