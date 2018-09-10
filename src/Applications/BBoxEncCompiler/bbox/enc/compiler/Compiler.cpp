/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::Compiler class.
 */

#include <bbox/enc/compiler/Compiler.h>
#include <bbox/enc/compiler/Parser.h>
#include <bbox/enc/compiler/TypeLibrary.h>

#include <memory>
#include <map>

namespace bbox {
namespace enc {
namespace compiler {

struct Compiler::Pimpl
{
	explicit Pimpl(std::vector<std::string> &error_strings)
		: errors(error_strings)
		, type_lib(std::make_shared<TypeLibrary>(errors))
	{
	}

	void RegisterNamespace(const Namespace::ptr &new_ptr)
	{
		type_lib->RegisterNamespace(new_ptr);
	}

	void RegisterType(const Type::ptr &type)
	{
		type_lib->RegisterType(type);
	}

	ErrorList errors;
	TypeLibrary::ptr type_lib;

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
	Namespace::ptr new_ptr = Namespace::ptr(new Namespace(m_pimpl->type_lib, std::move(name_list)));

	m_pimpl->RegisterNamespace(new_ptr);

	return new_ptr;
}

Struct::ptr Compiler::Builder::CreateStruct(const Namespace::ptr &ns, const Token &name, bool is_message)
{
	Struct::ptr new_ptr = Struct::ptr(new Struct(ns, name, is_message));

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

	for (const auto &entry : builder.m_pimpl->type_lib->m_namespaces)
	{
		if (!entry.second->Validate(builder.m_pimpl->errors))
			return false;
	}

	for (const auto &entry : builder.m_pimpl->type_lib->m_namespaces)
	{
		entry.second->GenerateOutputs(outputs);
	}

	return true;
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
