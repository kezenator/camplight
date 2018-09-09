/**
* @file
*
* Implementation file for the bbox::enc::compiler::Namespace class.
*/

#include <bbox/enc/compiler/Namespace.h>
#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

bool Namespace::Validate(ErrorList &errors)
{
	for (const auto &t_entry : m_types)
	{
		if (!t_entry.second->Validate(errors))
			return false;
	}

	return true;
}

void Namespace::GenerateOutputs(std::map<std::string, std::string> &outputs)
{
	for (const auto &l_entry : m_languages)
	{
		for (const auto &t_entry : m_types)
		{
			t_entry.second->GenerateOutputs(outputs, l_entry.second, l_entry.first);
		}
	}
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
