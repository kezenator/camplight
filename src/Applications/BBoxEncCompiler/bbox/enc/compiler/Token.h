/**
 * @file
 *
 * Header file for the bbox::enc::compiler::Token class.
 */

#ifndef __BBOX__ENC__COMPILER__TOKEN_H__
#define __BBOX__ENC__COMPILER__TOKEN_H__

#include <bbox/enc/compiler/File.h>

#include <string_view>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that stores a token.
 */
class Token
{
public:

	enum E_TYPE
	{
		END_OF_FILE,
		OPEN_CURLY_BRACE,
		CLOSE_CURLY_BRACE,
		OPEN_SQUARE_BRACKET,
		CLOSE_SQUARE_BRACKET,
		DOUBLE_COLON,
		SEMICOLON,
		LESS_THAN,
		GREATER_THAN,
		COMMA,
		STRING,
		IDENTIFIER,
		KEYWORD_NAMESPACE,
		KEYWORD_STRUCT,
		KEYWORD_ENUM,
		UNEXPECTED_CHARACTER,
		UNTERMINATED_STRING_LITERAL,
	};

	static const char *c_str(E_TYPE type);


	~Token() = default;
	Token(const Token &) = default;
	Token(Token &&) = default;
	Token &operator =(const Token &) = default;
	Token &operator =(Token &&) = default;

	Token()
		: m_file()
		, m_line(1)
		, m_column(1)
		, m_contents()
		, m_type(END_OF_FILE)
	{
	}

	Token(const File::ptr &file, size_t line, size_t column, size_t offset, size_t length, E_TYPE type)
		: m_file(file)
		, m_line(line)
		, m_column(column)
		, m_contents(file->GetContents().data() + offset, length)
		, m_type(type)
	{
	}

	const File::ptr &GetFile() const { return m_file; }
	std::string_view GetLineContents() const;
	size_t GetLineNum() const { return m_line; }
	size_t GetColumnNum() const { return m_column; }
	const std::string_view &GetContents() const { return m_contents; }
	E_TYPE GetType() const { return m_type; }
	std::string GetDescription() const;

	std::string DecodeContentsAsString() const;

	bool Matches(const char *str) const
	{
		return 0 == m_contents.compare(str);
	}

	bool Matches(char ch) const
	{
		return (m_contents.size() == 1)
			&& (m_contents[0] == ch);
	}

	bool Matches(E_TYPE type) const
	{
		return (m_type == type);
	}

private:
	File::ptr m_file;
	size_t m_line;
	size_t m_column;
	std::string_view m_contents;
	E_TYPE m_type;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TOKEN_H__
