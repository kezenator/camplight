/**
 * @file
 *
 * Implmentation file for the bbox::enc::compiler::Extern class.
 */

#include <bbox/enc/compiler/Extern.h>

namespace bbox {
namespace enc {
namespace compiler {

Extern::Extern(const Namespace::ptr &ns_ptr, const Token &name)
	: Type(ns_ptr, name)
{
}

bool Extern::Validate(ErrorList &errors)
{
    if (GetNamespace()->GeneratesLanguage(Namespace::TYPESCRIPT))
    {
        errors.AddError(GetName(), "extern types not yet supported in namespaces that generate Typescript output");
        return false;
    }

	return true;
}

void Extern::GenerateOutputs(std::map<std::string, std::string> & /*outputs*/, const std::string & /*path*/, Namespace::Language /*language*/) const
{
    // No outputs to generate
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
