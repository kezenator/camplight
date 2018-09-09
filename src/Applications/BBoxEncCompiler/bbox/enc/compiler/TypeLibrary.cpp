/**
 * @file
 *
 * Implementation file for the bbox::enc::compiler::TypeLibrary class.
 */

#include <bbox/enc/compiler/TypeLibrary.h>
#include <bbox/enc/compiler/Namespace.h>
#include <bbox/enc/compiler/Type.h>

namespace bbox {
namespace enc {
namespace compiler {

struct TypeLibrary::StringTypeInstance : public TypeInstance
{
	std::string GetTypescriptTypeName() const override
	{
		return "string";
	}

	std::string GetTypescriptDefaultValue() const
	{
		return "\"\"";
	}

	void AddCppHeaderIncludes(std::set<std::string> & /*includes*/) const
	{
		// None
	}
};

struct TypeLibrary::VectorTypeInstance : public TypeInstance
{
	TypeInstance::ptr m_param_type;

	VectorTypeInstance(const TypeInstance::ptr &param_type)
		: m_param_type(param_type)
	{
	}

	std::string GetTypescriptTypeName() const override
	{
		return m_param_type->GetTypescriptTypeName() + "[]";
	}

	std::string GetTypescriptDefaultValue() const
	{
		return "new Array(0)";
	}

	void AddCppHeaderIncludes(std::set<std::string> &includes) const
	{
		m_param_type->AddCppHeaderIncludes(includes);
	}
};

TypeLibrary::TypeLibrary(ErrorList &errors)
	: m_errors(errors)
	, m_namespaces()
{
}

TypeLibrary::~TypeLibrary()
{
}

void TypeLibrary::RegisterNamespace(const std::shared_ptr<Namespace> &new_ptr)
{
	std::string name = new_ptr->GetName().ToString();

	auto insert_result = m_namespaces.insert(std::make_pair(name, new_ptr));

	if (!insert_result.second)
	{
		m_errors.AddError(new_ptr->GetName().GetStartToken(), bbox::Format("Duplicate namespace \"%s\"", name));
		m_errors.AddError(insert_result.first->second->GetName().GetStartToken(), "Originally defined here");
	}
}

void TypeLibrary::RegisterType(const std::shared_ptr<Type> &type)
{
	auto insert_result = type->GetNamespace()->m_types.insert(std::make_pair(type->GetName().GetContents(), type));

	if (!insert_result.second)
	{
		m_errors.AddError(type->GetName(), bbox::Format("Duplicate type \"%s\"", type->GetName().GetContents()));
		m_errors.AddError(insert_result.first->second->GetName(), "Originally defined here");
	}
}

bool TypeLibrary::ResolveType(const TypeName &type_name, TypeInstance::ptr &result)
{
	std::string base_name = type_name.GetBaseName().ToString();

	if (base_name == "std::string")
	{
		if (type_name.HasTemplateParams())
		{
			m_errors.AddError(type_name.GetStartToken(), "Type \"std::string\" is not a template");
			return false;
		}

		result = std::make_shared<StringTypeInstance>();
		return true;
	}
	else if (base_name == "std::vector")
	{
		if (!type_name.HasTemplateParams()
			|| (type_name.GetTemplateParams().size() != 1))
		{
			m_errors.AddError(type_name.GetStartToken(), "Type \"std::vector\" must have one template parameter");
			return false;
		}

		TypeInstance::ptr param_type;

		if (!ResolveType(type_name.GetTemplateParams()[0], param_type))
			return false;

		result = std::make_shared<VectorTypeInstance>(param_type);
		return true;
	}

	for (const auto &ns_entry : m_namespaces)
	{
		const auto &type_map = ns_entry.second->m_types;

		auto it = type_map.find(base_name);

		if (it != type_map.end())
		{
			result = it->second;
			return true;
		}
	}

	m_errors.AddError(
		type_name.GetStartToken(),
		bbox::Format("Unknown type \"%s\"", type_name.ToString()));

	return false;
}

} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
