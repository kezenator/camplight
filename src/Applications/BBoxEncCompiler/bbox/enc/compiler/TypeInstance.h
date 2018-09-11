/**
 * @file
 *
 * Header file for the bbox::enc::compiler::TypeInstance class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_INSTANCE_H__
#define __BBOX__ENC__COMPILER__TYPE_INSTANCE_H__

#include <memory>
#include <set>
#include <string>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * Traits for a type.
 */
class TypeInstance
{
public:

	using ptr = std::shared_ptr<TypeInstance>;

	virtual ~TypeInstance()
	{
	}

	virtual std::string GetTypescriptTypeName() const = 0;
	virtual std::string GetTypescriptTypeLibraryConstructor() const = 0;
	virtual std::string GetTypescriptDefaultValue() const = 0;
	virtual void AddTypescriptReferences(std::set<std::string> &references) const = 0;
	virtual void AddCppHeaderIncludes(std::set<std::string> &includes) const = 0;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_INSTANCE_H__
