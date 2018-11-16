/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Extern class.
 */

#ifndef __BBOX__ENC__COMPILER__EXTERN_H__
#define __BBOX__ENC__COMPILER__EXTERN_H__

#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

// Forard declarations
class Compiler;

/**
 * An externally defined type.
 */
class Extern: public Type
{
private:
	friend class Compiler;

	Extern(const Namespace::ptr &ns_ptr, const Token &name);

public:
	using ptr = std::shared_ptr<Extern>;

	bool Validate(ErrorList &errors) override;
	void GenerateOutputs(std::map<std::string, std::string> &outputs, const std::string &path, Namespace::Language language) const override;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__EXTERN_H__
