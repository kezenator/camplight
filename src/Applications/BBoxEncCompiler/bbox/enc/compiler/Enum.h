/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Enum class.
 */

#ifndef __BBOX__ENC__COMPILER__ENUM_H__
#define __BBOX__ENC__COMPILER__ENUM_H__

#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

// Forard declarations
class Compiler;

/**
 * A user-defined enumeration.
 */
class Enum : public Type
{
private:
	friend class Compiler;

	Enum(const Namespace::ptr &ns_ptr, const Token &name);

public:
	using ptr = std::shared_ptr<Enum>;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__ENUM_H__
