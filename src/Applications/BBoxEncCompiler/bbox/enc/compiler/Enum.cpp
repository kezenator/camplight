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

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
