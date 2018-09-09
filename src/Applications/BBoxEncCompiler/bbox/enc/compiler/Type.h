/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Type class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_H__
#define __BBOX__ENC__COMPILER__TYPE_H__

#include <sstream>

#include <bbox/enc/compiler/ErrorList.h>
#include <bbox/enc/compiler/Namespace.h>
#include <bbox/enc/compiler/TypeInstance.h>
#include <bbox/enc/compiler/TypeName.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * Base class from which all types are derived.
 */
class Type: public TypeInstance
{
protected:
	Type(const Namespace::ptr &ns_ptr, const Token &name);

public:

	using ptr = std::shared_ptr<Type>;

	virtual ~Type();

	const Namespace::ptr &GetNamespace() const { return m_ns; }
	const Token &GetName() const { return m_name; }

	virtual bool Validate(ErrorList &errors) = 0;
	virtual void GenerateOutputs(std::map<std::string, std::string> &outputs, const std::string &path, Namespace::Language language) const = 0;
	std::string GetTypescriptTypeName() const override;
	std::string GetTypescriptDefaultValue() const override;
	void AddCppHeaderIncludes(std::set<std::string> &includes) const override;

protected:
	bool ResolveType(const TypeName &type_name, TypeInstance::ptr &result)
	{
		return m_ns->GetTypeLibrary()->ResolveType(type_name, result);
	}

	void GenerateCppHeaderHeader(std::stringstream &stream, const char *type) const;
	void GenerateCppHeaderFooter(std::stringstream &stream) const;
	void GenerateCppSourceHeader(std::stringstream &stream, const char *type) const;
	void GenerateCppNamespaceOpen(std::stringstream &stream) const;
	void GenerateCppNamespaceClose(std::stringstream &stream) const;

private:

	void GenerateCppHeaderGuard(std::stringstream &stream) const;

	Namespace::ptr m_ns;
	Token m_name;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_H__
