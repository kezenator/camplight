/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Parser class.
 */

#ifndef __BBOX__ENC__COMPILER__PARSER_H__
#define __BBOX__ENC__COMPILER__PARSER_H__

#include <bbox/enc/compiler/File.h>
#include <bbox/enc/compiler/ErrorList.h>
#include <bbox/enc/compiler/Compiler.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that parses tokens into an AST.
 */
class Parser
{
	Parser() = delete;
	~Parser() = delete;

public:

	static bool ParseFile(Compiler::Builder &builder, const File::ptr &file, ErrorList &errors);

private:
	struct Pimpl;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__PARSER_H__
