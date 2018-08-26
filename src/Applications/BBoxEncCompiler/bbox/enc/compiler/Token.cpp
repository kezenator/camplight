/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Token class.
 */

#include <bbox/enc/compiler/Token.h>
#include <bbox/Assert.h>
#include <bbox/Format.h>

namespace bbox {
namespace enc {
namespace compiler {

const char *Token::c_str(E_TYPE type)
{
	switch (type)
	{
	case END_OF_FILE:
		return "End-of-file";

	case OPEN_CURLY_BRACE:
		return "\'{\'";
	case CLOSE_CURLY_BRACE:
		return "\'}\'";
	case SEMICOLON:
		return "\';\'";
	case DOUBLE_COLON:
		return "\"::\"";
	case IDENTIFIER:
		return "identifier";
	case KEYWORD_NAMESPACE:
		return "keyword \"namespace\"";
	case KEYWORD_STRUCT:
		return "keyword \"struct\"";
	case KEYWORD_ENUM:
		return "keyword \"enum\"";
	case UNEXPECTED_CHARACTER:
		return "unexpected character";
	}
	return "unknown token";
}

std::string Token::GetDescription() const
{
	switch (m_type)
	{
	case END_OF_FILE:
	case OPEN_CURLY_BRACE:
	case CLOSE_CURLY_BRACE:
	case DOUBLE_COLON:
	case SEMICOLON:
	case KEYWORD_NAMESPACE:
	case KEYWORD_STRUCT:
	case KEYWORD_ENUM:
		// Just the type describes the contents
		return c_str(m_type);

	case IDENTIFIER:
	case UNEXPECTED_CHARACTER:
		// default - below
		break;
	}

	return bbox::Format("%s \"%s\"", c_str(m_type), m_contents);
}

std::string_view Token::GetLineContents() const
{
	if (!m_file)
		return std::string_view();

	// Our contents is a string view into the file contents.
	// Work out the offset in this data.

	const char *str_ptr = m_file->GetContents().data();
	size_t str_len = m_file->GetContents().size();

	size_t offset = m_contents.data() - str_ptr;

	BBOX_ASSERT(offset >= 0);
	BBOX_ASSERT(offset < m_file->GetContents().size());

	// Search backwards and forwards for new line characters
	// to find the line

	size_t start_offset = offset;

	while ((start_offset > 0)
		&& (str_ptr[start_offset - 1] != '\n'))
	{
		start_offset -= 1;
	}

	size_t end_offset = offset + 1;

	while ((end_offset < str_len)
		&& (str_ptr[end_offset] != '\n'))
	{
		end_offset += 1;
	}

	// Return a string view into the data

	return std::string_view(str_ptr + start_offset, end_offset - start_offset);
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
