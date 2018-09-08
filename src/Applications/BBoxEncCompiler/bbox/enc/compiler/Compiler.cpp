/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Compiler class.
 */

#include <bbox/enc/compiler/Compiler.h>
#include <bbox/enc/compiler/Parser.h>

#include <memory>
#include <map>

namespace bbox {
namespace enc {
namespace compiler {

struct Compiler::Pimpl
{
	explicit Pimpl(std::vector<std::string> &error_strings)
		: errors(error_strings)
	{
	}

	void RegisterNamespace(const Namespace::ptr &new_ptr)
	{
		std::string name = new_ptr->GetName().ToString();

		auto insert_result = m_namespaces.insert(std::make_pair(name, new_ptr));

		if (!insert_result.second)
		{
			errors.AddError(new_ptr->GetName().GetStartToken(), bbox::Format("Duplicate namespace \"%s\"", name));
			errors.AddError(insert_result.first->second->GetName().GetStartToken(), "Originally defined here");
		}
	}

	void RegisterType(const Type::ptr &type)
	{
		auto insert_result = type->GetNamespace()->m_types.insert(std::make_pair(type->GetName().GetContents(), type));

		if (!insert_result.second)
		{
			errors.AddError(type->GetName(), bbox::Format("Duplicate type \"%s\"", type->GetName().GetContents()));
			errors.AddError(insert_result.first->second->GetName(), "Originally defined here");
		}
	}

	ErrorList errors;
	std::map<std::string, Namespace::ptr> m_namespaces;
};

Compiler::Builder::Builder(std::vector<std::string> &error_strings)
	: m_pimpl(std::make_unique<Pimpl>(error_strings))
{
}

Compiler::Builder::~Builder()
{
}

Namespace::ptr Compiler::Builder::CreateNamespace(TypeNameList &&name_list)
{
	Namespace::ptr new_ptr = Namespace::ptr(new Namespace(std::move(name_list)));

	m_pimpl->RegisterNamespace(new_ptr);

	return new_ptr;
}

Struct::ptr Compiler::Builder::CreateStruct(const Namespace::ptr &ns, const Token &name)
{
	Struct::ptr new_ptr = Struct::ptr(new Struct(ns, name));

	m_pimpl->RegisterType(new_ptr);

	return new_ptr;
}

Enum::ptr Compiler::Builder::CreateEnum(const Namespace::ptr &ns, const Token &name)
{
	Enum::ptr new_ptr = Enum::ptr(new Enum(ns, name));

	m_pimpl->RegisterType(new_ptr);

	return new_ptr;
}

bool Compiler::CompileSources(
	std::vector<File::ptr> &&sources,
	std::map<std::string, std::string> &outputs, 
	std::vector<std::string> &error_strings)
{
	Builder builder(error_strings);

	for (const auto &file_ptr : sources)
	{
		if (!Parser::ParseFile(builder, file_ptr, builder.m_pimpl->errors))
			return false;
	}

	for (const auto &entry : builder.m_pimpl->m_namespaces)
	{
		entry.second->GenerateOutputs(outputs);
	}

	return true;
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
