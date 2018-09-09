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

	explicit TypeName(TypeNameList &&type_name, std::optional<std::vector<TypeName>> &&opt_template_params)
		: m_type_name(std::move(type_name))
		, m_opt_template_params(std::move((opt_template_params)))
	{
	}

	const Token &GetStartToken() const
	{
		return m_type_name.GetStartToken();
	}

	const TypeNameList &GetBaseName() const
	{
		return m_type_name;
	}

	bool HasTemplateParams() const
	{
		return m_opt_template_params.has_value();
	}

	const std::vector<TypeName> &GetTemplateParams() const
	{
		return m_opt_template_params.value();
	}

	std::string ToString() const
	{
		std::string result = m_type_name.ToString();

		if (m_opt_template_params)
		{
			result.push_back('<');

			bool first = true;

			for (const auto &param : *m_opt_template_params)
			{
				if (first)
					first = false;
				else
					result.append(", ");

				result.append(param.ToString());
			}

			result.push_back('>');
		}

		return result;
	}

private:
	TypeNameList m_type_name;
	std::optional<std::vector<TypeName>> m_opt_template_params;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_NAME_H__
