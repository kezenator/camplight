/**
 * @file
 *
 * Implmentation file for the bbox::enc::compiler::Enum class.
 */

#include <bbox/enc/compiler/Enum.h>

namespace bbox {
namespace enc {
namespace compiler {

Enum::Enum(const Namespace::ptr &ns_ptr, const Token &name)
	: Type(ns_ptr, name)
{
}

bool Enum::Validate(ErrorList & /*errors*/)
{
	// Nothing to do

	return true;
}

void Enum::GenerateOutputs(std::map<std::string, std::string> &outputs, const std::string &path, Namespace::Language language) const
{
	// TODO
	(void)outputs;
	(void)path;
	(void)language;
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
