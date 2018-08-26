/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Type class.
 */

#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

Type::Type(const Namespace::ptr &ns_ptr, const Token &name)
	: m_ns(ns_ptr)
	, m_name(name)
{
}

Type::~Type()
{
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
