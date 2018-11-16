/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Compiler class.
 */

#ifndef __BBOX__ENC__COMPILER__COMPILER_H__
#define __BBOX__ENC__COMPILER__COMPILER_H__

#include <vector>

#include <bbox/enc/compiler/Namespace.h>
#include <bbox/enc/compiler/Struct.h>
#include <bbox/enc/compiler/Enum.h>
#include <bbox/enc/compiler/Extern.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * Top level class that compiles BBox IDL files
 * and produces the output.
 */
class Compiler
{
private:
	struct Pimpl;

public:

	/**
	 * A handle to build objects during the parse phase.
	 */
	class Builder
	{
		friend class Compiler;

	public:
		Namespace::ptr CreateNamespace(TypeNameList &&name_list);
		Struct::ptr CreateStruct(const Namespace::ptr &ns, const Token &name, bool is_message);
		Enum::ptr CreateEnum(const Namespace::ptr &ns, const Token &name);
        Extern::ptr CreateExtern(const Namespace::ptr &ns, const Token &name);

	private:
		Builder() = delete;
		explicit Builder(std::vector<std::string> &error_strings);
		~Builder();

		std::unique_ptr<Pimpl> m_pimpl;
	};

	static bool CompileSources(
		std::vector<File::ptr> &&sources,
		std::map<std::string, std::string> &outputs,
		std::vector<std::string> &error_strings);

};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__COMPILER_H__
