/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Struct class.
 */

#ifndef __BBOX__ENC__COMPILER__STRUCT_H__
#define __BBOX__ENC__COMPILER__STRUCT_H__

#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

// Forard declarations
class Compiler;

/**
 * A user-defined structure.
 */
class Struct: public Type
{
private:
	friend class Compiler;

	Struct(const Namespace::ptr &ns_ptr, const Token &name);

public:
	using ptr = std::shared_ptr<Struct>;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__STRUCT_H__
