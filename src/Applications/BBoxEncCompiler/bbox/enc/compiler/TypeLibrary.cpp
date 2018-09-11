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

struct TypeLibrary::SimpleTypeInstance : public TypeInstance
{
	std::string m_typescript_type;
	std::string m_typescript_lib_constructor;
	std::string m_typescript_default_value;

	SimpleTypeInstance(std::string &&typescript_type, std::string &&typescript_lib_constructor, std::string &&typescript_default_value)
		: m_typescript_type(std::move(typescript_type))
		, m_typescript_lib_constructor(std::move(typescript_lib_constructor))
		, m_typescript_default_value(std::move(typescript_default_value))
	{
	}
		
	std::string GetTypescriptTypeName() const override
	{
		return m_typescript_type;
	}

	std::string GetTypescriptTypeLibraryConstructor() const override
	{
		return m_typescript_lib_constructor;
	}

	std::string GetTypescriptDefaultValue() const override
	{
		return m_typescript_default_value;
	}

	void AddTypescriptReferences(std::set<std::string> & /*references*/) const override
	{
		// None
	}

	void AddCppHeaderIncludes(std::set<std::string> & /*includes*/) const override
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
		return bbox::Format("bbox.ds.Deque<%s>", m_param_type->GetTypescriptTypeName());
	}

	std::string GetTypescriptTypeLibraryConstructor() const override
	{
		return bbox::Format("bbox.enc.TypeLibrary.stdVectorAsDeque(%s)", m_param_type->GetTypescriptTypeLibraryConstructor());
	}

	std::string GetTypescriptDefaultValue() const override
	{
		return bbox::Format("new bbox.ds.Deque<%s>()", m_param_type->GetTypescriptTypeName());
	}

	void AddTypescriptReferences(std::set<std::string> &references) const override
	{
		m_param_type->AddTypescriptReferences(references);
	}

	void AddCppHeaderIncludes(std::set<std::string> &includes) const override
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

		result = std::make_shared<SimpleTypeInstance>("string", "\"std::string\"", "\"\"");
		return true;
	}
	else if (base_name == "bool")
	{
		if (type_name.HasTemplateParams())
		{
			m_errors.AddError(type_name.GetStartToken(), "Type \"bool\" is not a template");
			return false;
		}

		result = std::make_shared<SimpleTypeInstance>("boolean", "\"bool\"", "false");
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
