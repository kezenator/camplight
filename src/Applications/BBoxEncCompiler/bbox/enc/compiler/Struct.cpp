/**
 * @file
 *
 * Implmentation file for the bbox::enc::compiler::Struct class.
 */

#include <bbox/enc/compiler/Struct.h>

namespace bbox {
namespace enc {
namespace compiler {

Struct::Struct(const Namespace::ptr &ns_ptr, const Token &name)
	: Type(ns_ptr, name)
{
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
