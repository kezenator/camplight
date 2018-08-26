/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Tokenizer class.
 */

#ifndef __BBOX__ENC__COMPILER__TOKENIZER_H__
#define __BBOX__ENC__COMPILER__TOKENIZER_H__

#include <bbox/enc/compiler/Token.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that tokenizer strings into tokens.
 */
class Tokenizer
{
public:

	Tokenizer() = delete;
	Tokenizer(const Tokenizer &) = delete;
	Tokenizer &operator =(Tokenizer &) = delete;

	Tokenizer(const File::ptr &file);
	~Tokenizer() = default;

	Token NextToken();

private:
	File::ptr m_file;
	std::string_view m_contents;
	size_t m_cur_line;
	size_t m_cur_col;
	size_t m_cur_offset;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TOKENIZER_H__
