/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Type class.
 */

#ifndef __BBOX__ENC__COMPILER__TYPE_H__
#define __BBOX__ENC__COMPILER__TYPE_H__

#include <bbox/enc/compiler/Namespace.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * Base class from which all types are derived.
 */
class Type
{
protected:
	Type(const Namespace::ptr &ns_ptr, const Token &name);

public:

	using ptr = std::shared_ptr<Type>;

	virtual ~Type();

	const Namespace::ptr &GetNamespace() const { return m_ns; }
	const Token &GetName() const { return m_name; }

private:
	Namespace::ptr m_ns;
	Token m_name;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TYPE_H__
