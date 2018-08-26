/**
 * @file
 *
 * Header file for the bbox::enc::compiler::ErrorList class.
 */

#ifndef __BBOX__ENC__COMPILER__ERROR_LIST_H__
#define __BBOX__ENC__COMPILER__ERROR_LIST_H__

#include <string>
#include <vector>

#include <bbox/Format.h>
#include <bbox/enc/compiler/Token.h>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A clas that builds error messages.
 */
class ErrorList
{
public:

	ErrorList(std::vector<std::string> &result)
		: m_result(result)
	{
	}

	void AddError(const File::ptr &file, size_t line, size_t column, const std::string_view &line_contents, const std::string &error)
	{
		m_result.emplace_back(bbox::Format("%s:%d: %s", file->GetName(), line, error));
		m_result.emplace_back(line_contents);
		m_result.emplace_back(bbox::Format("%s^", std::string(column - 1, ' ')));
	}

	void AddError(const Token &position, const std::string &error)
	{
		AddError(position.GetFile(), position.GetLineNum(), position.GetColumnNum(), position.GetLineContents(), error);
	}

private:
	std::vector<std::string> &m_result;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__ERROR_LIST_H__
