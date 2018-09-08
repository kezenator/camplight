/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Namespace class.
 */

#ifndef __BBOX__ENC__COMPILER__NAMESPACE_H__
#define __BBOX__ENC__COMPILER__NAMESPACE_H__

#include <memory>
#include <map>

#include <bbox/enc/compiler/TypeNameList.h>

namespace bbox {
namespace enc {
namespace compiler {

// Forward declarations;
class Compiler;
class Type;

/**
 * Describes a namespace
 */
class Namespace
{
	friend class Compiler;

	Namespace() = delete;

public:
	using ptr = std::shared_ptr<Namespace>;

	enum Language
	{
		CPP,
		TYPESCRIPT,
	};

	~Namespace() = default;

	const TypeNameList &GetName() const { return m_name; }

	bool GeneratesLanguage(Language l) const
	{
		return m_languages.count(l) > 0;
	}

	void AddLanguage(Language l, std::string &&path)
	{
		m_languages[l] = std::move(path);
	}

	void GenerateOutputs(std::map<std::string, std::string> &outputs);

private:
	explicit Namespace(TypeNameList &&name)
		: m_name(std::move(name))
	{
	}

	TypeNameList m_name;
	std::map<std::string_view, std::shared_ptr<Type>> m_types;
	std::map<Language, std::string> m_languages;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__NAMESPACE_H__
