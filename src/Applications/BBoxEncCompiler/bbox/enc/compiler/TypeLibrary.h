/**
 * @file
 *
 * Header file for the bbox::enc::compiler::TypeLibrary class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_LIBRARY_H__
#define __BBOX__ENC__COMPILER__TYPE_LIBRARY_H__

#include <map>
#include <memory>

#include <bbox/enc/compiler/TypeName.h>
#include <bbox/enc/compiler/TypeInstance.h>

namespace bbox {
namespace enc {
namespace compiler {

// Forward declarations
class Compiler;
class ErrorList;
class Namespace;
class Type;

/**
 * The library of types.
 */
class TypeLibrary
{
	friend class Compiler;

public:

	using ptr = std::shared_ptr<TypeLibrary>;
	
	explicit TypeLibrary(ErrorList &errors);
	~TypeLibrary();

	bool ResolveType(const TypeName &type_name, TypeInstance::ptr &result);

private:
	void RegisterNamespace(const std::shared_ptr<Namespace> &new_ptr);
	void RegisterType(const std::shared_ptr<Type> &type);

	struct StringTypeInstance;
	struct VectorTypeInstance;

	ErrorList &m_errors;
	std::map<std::string, std::shared_ptr<Namespace>> m_namespaces;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_LIBRARY_H__
