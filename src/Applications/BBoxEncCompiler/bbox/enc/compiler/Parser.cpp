/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Parser class.
 */

#include <bbox/enc/compiler/Parser.h>
#include <bbox/enc/compiler/Tokenizer.h>

#include <sstream>
#include <optional>

namespace bbox {
namespace enc {
namespace compiler {

struct Parser::Pimpl
{
	Compiler::Builder &m_builder;
	Tokenizer m_toks;
	ErrorList &m_errors;
	std::optional<Token> m_opt_peeked;

	Pimpl(Compiler::Builder &builder, const File::ptr &file, ErrorList &errors)
		: m_builder(builder)
		, m_toks(file)
		, m_errors(errors)
		, m_opt_peeked(std::nullopt)
	{
	}

	Token PeekToken()
	{
		if (!m_opt_peeked)
			m_opt_peeked = m_toks.NextToken();
		return *m_opt_peeked;
	}

	Token NextToken()
	{
		Token result;

		if (m_opt_peeked)
		{
			result = *m_opt_peeked;
			m_opt_peeked = std::nullopt;
		}
		else
		{
			result = m_toks.NextToken();
		}

		return result;
	}

	bool Expect(Token &tok, Token::E_TYPE type)
	{
		tok = NextToken();

		if (!tok.Matches(type))
		{
			m_errors.AddError(tok, bbox::Format(
				"Expected %s but found %s instead",
				Token::c_str(type),
				tok.GetDescription()));

			return false;
		}
		return true;
	}

	bool Expect(Token::E_TYPE type)
	{
		Token tok;
		return Expect(tok, type);
	}

	bool Expect(Token &tok, const std::initializer_list<Token::E_TYPE> &types)
	{
		tok = NextToken();

		for (Token::E_TYPE type : types)
		{
			if (tok.Matches(type))
				return true;
		}

		bool first = true;
		std::stringstream stream;

		stream << "Expected one of [";

		for (Token::E_TYPE type : types)
		{
			if (first)
				first = false;
			else
				stream << ", ";
			stream << Token::c_str(type);
		}
		stream << "] but found " << tok.GetDescription() << " instead";

		m_errors.AddError(tok, stream.str());

		return false;
	}

	bool Try(Token::E_TYPE type)
	{
		return PeekToken().Matches(type);
	}

	bool ParseTypeNameList(TypeNameList &result)
	{
		std::vector<Token> tokens;

		while (true)
		{
			tokens.emplace_back();

			if (!Expect(tokens.back(), Token::IDENTIFIER))
				return false;

			if (!Try(Token::DOUBLE_COLON))
			{
				result = TypeNameList(std::move(tokens));
				return true;
			}

			if (!Expect(Token::DOUBLE_COLON))
				return false;
		}
	}

	bool ParseTypeName(TypeName &result)
	{
		TypeNameList base_name;
		std::optional<std::vector<TypeName>> opt_template_params;

		if (!ParseTypeNameList(base_name))
			return false;

		if (Try(Token::LESS_THAN))
		{
			if (!Expect(Token::LESS_THAN))
				return false;

			opt_template_params.emplace();

			while (true)
			{
				opt_template_params->emplace_back();

				if (!ParseTypeName(opt_template_params->back()))
					return false;

				Token next_tok;
				if (!Expect(next_tok, { Token::COMMA, Token::GREATER_THAN }))
					return false;

				if (next_tok.Matches(Token::GREATER_THAN))
					break;
			}
		}

		result = TypeName(std::move(base_name), std::move(opt_template_params));

		return true;
	}

	bool ParseFile()
	{
		if (!Expect(Token::KEYWORD_NAMESPACE))
			return false;

		Namespace::ptr namespace_ptr;

		{
			TypeNameList namespace_name;
			if (!ParseTypeNameList(namespace_name))
				return false;

			namespace_ptr = m_builder.CreateNamespace(std::move(namespace_name));
		}

		while (true)
		{
			Token tok;
			if (!Expect(tok, { Token::OPEN_CURLY_BRACE, Token::OPEN_SQUARE_BRACKET }))
				return false;

			if (tok.Matches(Token::OPEN_CURLY_BRACE))
				break;

			Token attribute;
			if (!Expect(attribute, Token::IDENTIFIER))
				return false;

			if (!attribute.Matches("generate"))
			{
				m_errors.AddError(attribute, "Only \"generate\" attribute supported");
				return false;
			}

			Token language_tok;
			if (!Expect(language_tok, Token::IDENTIFIER))
				return false;

			Namespace::Language language;

			if (language_tok.Matches("cpp"))
			{
				language = Namespace::CPP;
			}
			else if (language_tok.Matches("ts"))
			{
				language = Namespace::TYPESCRIPT;
			}
			else
			{
				m_errors.AddError(language_tok, "Expected \"cpp\" or \"ts\"");
				return false;
			}

			if (namespace_ptr->GeneratesLanguage(language))
			{
				m_errors.AddError(language_tok, "Duplicate generate language");
				return false;
			}

			Token path;
			if (!Expect(path, Token::STRING))
				return false;

			namespace_ptr->AddLanguage(language, path.DecodeContentsAsString());

			if (!Expect(Token::CLOSE_SQUARE_BRACKET))
				return false;
		}

		while (true)
		{
			Token tok;
			if (!Expect(tok, { Token::CLOSE_CURLY_BRACE,
							   Token::KEYWORD_STRUCT,
							   Token::KEYWORD_MESSAGE,
							   Token::KEYWORD_ENUM,
                               Token::KEYWORD_EXTERN }))
			{
				return false;
			}

			if (tok.Matches(Token::CLOSE_CURLY_BRACE))
			{
				break;
			}
			else if (tok.Matches(Token::KEYWORD_STRUCT)
				|| tok.Matches(Token::KEYWORD_MESSAGE))
			{
				Token name_tok;
				if (!Expect(name_tok, Token::IDENTIFIER))
					return false;

                if (!Expect(Token::OPEN_CURLY_BRACE))
					return false;

				Struct::ptr struct_ptr = m_builder.CreateStruct(
					namespace_ptr,
					name_tok,
					tok.Matches(Token::KEYWORD_MESSAGE));

				while (true)
				{
					Token peek_tok = PeekToken();
					if (peek_tok.Matches(Token::CLOSE_CURLY_BRACE))
					{
						break;
					}
					else if (!peek_tok.Matches(Token::IDENTIFIER))
					{
						Expect(peek_tok, { Token::IDENTIFIER, Token::CLOSE_CURLY_BRACE });
						return false;
					}
					else
					{
						TypeName type_name;

						if (!ParseTypeName(type_name))
							return false;

						Token field_tok;
						if (!Expect(field_tok, Token::IDENTIFIER))
							return false;
						if (!Expect(Token::SEMICOLON))
							return false;

						struct_ptr->AddField(type_name, field_tok);
					}
				}
				if (!Expect(Token::CLOSE_CURLY_BRACE))
					return false;
				if (!Expect(Token::SEMICOLON))
					return false;
			}
			else if (tok.Matches(Token::KEYWORD_ENUM))
			{
				// TODO
				Token name_tok;
				if (!Expect(name_tok, Token::IDENTIFIER))
					return false;
				if (!Expect(Token::OPEN_CURLY_BRACE))
					return false;
				if (!Expect(Token::CLOSE_CURLY_BRACE))
					return false;
				if (!Expect(Token::SEMICOLON))
					return false;

				m_builder.CreateEnum(namespace_ptr, name_tok);
			}
            else if (tok.Matches(Token::KEYWORD_EXTERN))
            {
                Token name_tok;
                if (!Expect(name_tok, Token::IDENTIFIER))
                    return false;
                if (!Expect(Token::SEMICOLON))
                    return false;

                m_builder.CreateExtern(namespace_ptr, name_tok);
            }
		}

		if (!Expect(Token::END_OF_FILE))
			return false;

		return true;
	}
};

bool Parser::ParseFile(Compiler::Builder &builder, const File::ptr &file, ErrorList &errors)
{
	return Pimpl(builder, file, errors).ParseFile();
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
