/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Tokenizer class.
 */

#include <bbox/enc/compiler/Tokenizer.h>

namespace bbox {
namespace enc {
namespace compiler {

Tokenizer::Tokenizer(const File::ptr &file)
	: m_file(file)
	, m_contents(file->GetContents())
	, m_cur_line(1)
	, m_cur_col(1)
	, m_cur_offset(0)
{
}

Token Tokenizer::NextToken()
{
	// Skip whitespace

	while (m_cur_offset < m_contents.size())
	{
		char ch = m_contents[m_cur_offset];

		if ((ch == ' ')
			|| (ch == '\t')
			|| (ch == '\r'))
		{
			m_cur_offset += 1;
			m_cur_col += 1;
		}
		else if (ch == '\n')
		{
			m_cur_offset += 1;
			m_cur_line += 1;
			m_cur_col = 1;
		}
		else
		{
			break;
		}
	}

	// Decode the next token

	Token result{ m_file, m_cur_line, m_cur_col, m_cur_offset, 0, Token::END_OF_FILE };

	if (m_cur_offset < m_contents.size())
	{
		char start_ch = m_contents[m_cur_offset];

		Token::E_TYPE type = Token::UNEXPECTED_CHARACTER;
		size_t length = 1;

		if (start_ch == '{')
		{
			type = Token::OPEN_CURLY_BRACE;
			length = 1;
		}
		else if (start_ch == '}')
		{
			type = Token::CLOSE_CURLY_BRACE;
			length = 1;
		}
		else if (start_ch == '[')
		{
			type = Token::OPEN_SQUARE_BRACKET;
			length = 1;
		}
		else if (start_ch == ']')
		{
			type = Token::CLOSE_SQUARE_BRACKET;
			length = 1;
		}
		else if (start_ch == ';')
		{
			type = Token::SEMICOLON;
			length = 1;
		}
		else if (start_ch == '<')
		{
			type = Token::LESS_THAN;
			length = 1;
		}
		else if (start_ch == '>')
		{
			type = Token::GREATER_THAN;
			length = 1;
		}
		else if (start_ch == ',')
		{
			type = Token::COMMA;
			length = 1;
		}
		else if (start_ch == ':')
		{
			if (((m_cur_offset + 1) < m_contents.size())
				&& (m_contents[m_cur_offset + 1] == ':'))
			{
				type = Token::DOUBLE_COLON;
				length = 2;
			}
		}
		else if (((start_ch >= 'A') && (start_ch <= 'Z'))
			|| ((start_ch >= 'a') && (start_ch <= 'z'))
			|| (start_ch == '_'))
		{
			size_t end_offset = m_cur_offset + 1;
			while (true)
			{
				if (end_offset >= m_contents.size())
					break;

				char ch = m_contents[end_offset];

				if (((ch >= 'A') && (ch <= 'Z'))
					|| ((ch >= 'a') && (ch <= 'z'))
					|| ((ch >= '0') && (ch <= '9'))
					|| (ch == '_'))
				{
					end_offset += 1;
				}
				else
				{
					break;
				}
			}

			type = Token::IDENTIFIER;
			length = end_offset - m_cur_offset;

			std::string_view strv = m_contents.substr(m_cur_offset, length);

			if (strv == "namespace")
				type = Token::KEYWORD_NAMESPACE;
			else if (strv == "struct")
				type = Token::KEYWORD_STRUCT;
			else if (strv == "message")
				type = Token::KEYWORD_MESSAGE;
			else if (strv == "enum")
				type = Token::KEYWORD_ENUM;
		}
		else if (start_ch == '\"')
		{
			type = Token::UNTERMINATED_STRING_LITERAL;

			size_t end_offset = m_cur_offset + 1;
			while (true)
			{
				if (end_offset >= m_contents.size())
					break;

				char ch = m_contents[end_offset];

				if (ch == '\n')
				{
					// Unterminated - can't span multiple lines
					break;
				}
				else if (ch == '\"')
				{
					type = Token::STRING;
					end_offset += 1;
					break;
				}

				end_offset += 1;
			}

			length = end_offset - m_cur_offset;
		}

		result = Token(m_file, m_cur_line, m_cur_col, m_cur_offset, length, type);

		m_cur_col += length;
		m_cur_offset += length;
	}

	return result;
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
