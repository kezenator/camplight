/**
 * @file
 *
 * Header file for the bbox::enc::compiler::File class.
 */

#ifndef __BBOX__ENC__COMPILER__FILE_H__
#define __BBOX__ENC__COMPILER__FILE_H__

#include <memory>
#include <string>

namespace bbox {
namespace enc {
namespace compiler {

/**
 * A class that stores a file.
 */
class File
{
public:

	using ptr = std::shared_ptr<File>;

	File() = delete;
	File(const File &) = delete;
	File(File &&) = delete;
	File &operator =(const File &) = delete;
	File &operator =(File &&) = delete;

	File(std::string &&name, std::string &&contents)
		: m_name(std::move(name))
		, m_contents(std::move(contents))
	{
	}

	~File() = default;

	const std::string &GetName() const { return m_name; }
	const std::string &GetContents() const { return m_contents; }

private:
	std::string m_name;
	std::string m_contents;
};

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__COMPILER__TOKEN_H__
