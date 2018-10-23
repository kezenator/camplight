/**
 * @file
 *
 * Implmentation file for the bbox::enc::compiler::Struct class.
 */

#include <bbox/enc/compiler/Struct.h>

namespace bbox {
namespace enc {
namespace compiler {

Struct::Struct(const Namespace::ptr &ns_ptr, const Token &name, bool is_message)
	: Type(ns_ptr, name)
	, m_is_message(is_message)
{
}

void Struct::AddField(const TypeName &type, const Token &name)
{
	m_fields.emplace_back();
	m_fields.back().type = type;
	m_fields.back().name = name;
}

bool Struct::Validate(ErrorList &errors)
{
	for (Field &field : m_fields)
	{
		if (!ResolveType(field.type, field.instance))
			return false;
	}

	return true;
}

void Struct::GenerateOutputs(std::map<std::string, std::string> &outputs, const std::string &path, Namespace::Language language) const
{
	std::string base_path = path;
	base_path.push_back('/');
	base_path.append(GetName().GetContents());

	switch (language)
	{
	case Namespace::CPP:
		outputs[base_path + ".h"] = GenerateCppHeader();
		outputs[base_path + ".cpp"] = GenerateCppSource();
		break;

	case Namespace::TYPESCRIPT:
		outputs[base_path + ".ts"] = GenerateTypescript();
		break;
	}
}

std::string Struct::GenerateCppHeader() const
{
	std::stringstream stream;

	GenerateCppHeaderHeader(stream, "struct");

	{
		std::set<std::string> headers;
		for (const Field &field : m_fields)
			field.instance->AddCppHeaderIncludes(headers);

		if (!headers.empty())
		{
			for (const auto &header : headers)
				stream << header << std::endl;
			stream << std::endl;
		}
	}

	GenerateCppNamespaceOpen(stream);

	std::string_view name = GetName().GetContents();

	stream << "class " << name << std::endl;
	stream << '{' << std::endl;
	stream << "public:" << std::endl;
	stream << "    " << name << "() = default;" << std::endl;
	stream << "    ~" << name << "() = default;" << std::endl;
	stream << "    " << name << "(const " << name << "&) = default;" << std::endl;
	stream << "    " << name << "(" << name << "&&) = default;" << std::endl;
	stream << "    " << name << " &operator =(const " << name << "&) = default;" << std::endl;
	stream << "    " << name << " &operator =(" << name << "&&) = default;" << std::endl;
	stream << std::endl;
	stream << "    void ToBinary(bbox::enc::ToBinary &m) const;" << std::endl;
	stream << "    void FromBinary(bbox::enc::FromBinary &m);" << std::endl;
	stream << "    void ToTextFormat(bbox::enc::ToTextFormat &m) const;" << std::endl;
	stream << "    void FromTextFormat(bbox::enc::FromTextFormat &m);" << std::endl;

	if (!m_fields.empty())
	{
		stream << std::endl;

		for (const Field &field : m_fields)
		{
			stream << "    " << field.type.ToString() << " " << field.name.GetContents() << ';' << std::endl;
		}
	}

	if (m_is_message)
	{
		stream << std::endl;
		stream << "    static bbox::enc::MsgTypeLibrary::Registration<"
			<< name << "> g_msg_registration;" << std::endl;
	}

	stream << "};" << std::endl;
	stream << std::endl;

	GenerateCppNamespaceClose(stream);
	GenerateCppHeaderFooter(stream);

	return stream.str();
}

std::string Struct::GenerateCppSource() const
{
	std::stringstream stream;

	std::string_view name = GetName().GetContents();

	GenerateCppSourceHeader(stream, "struct");

	stream << "#include <";
	for (const Token &tok : GetNamespace()->GetName().GetTokens())
		stream << tok.GetContents() << "/";
	stream << name << ".h>" << std::endl;
	stream << std::endl;

	GenerateCppNamespaceOpen(stream);

	auto generate_marshaling = [&](const std::string_view &cmd, bool text_format)
	{
		stream << "{" << std::endl;

		if (text_format)
			stream << "    m.StartStructure();" << std::endl;

		for (const Field &field : m_fields)
		{
			stream << "    m." << cmd << '(';
			if (text_format)
				stream << '\"' << field.name.GetContents() << "\", ";
			stream << field.name.GetContents() << ");" << std::endl;
		}

		if (text_format)
			stream << "    m.CompleteStructure();" << std::endl;

		if (!text_format
			&& m_fields.empty())
		{
			stream << "    (void)m;" << std::endl;
		}

		stream << "}" << std::endl;
		stream << std::endl;
	};

	stream << "void " << name << "::ToBinary(bbox::enc::ToBinary &m) const" << std::endl;
	generate_marshaling("Write", false);
	stream << "void " << name << "::FromBinary(bbox::enc::FromBinary &m)" << std::endl;
	generate_marshaling("Read", false);
	stream << "void " << name << "::ToTextFormat(bbox::enc::ToTextFormat &m) const" << std::endl;
	generate_marshaling("AddNamedValue", true);
	stream << "void " << name << "::FromTextFormat(bbox::enc::FromTextFormat &m)" << std::endl;
	generate_marshaling("DecodeNamedValue", true);

	if (m_is_message)
	{
		stream << "bbox::enc::MsgTypeLibrary::Registration<"
			<< name << ">" << std::endl;
		stream << "    " << name << "::g_msg_registration(\""
			<< GetNamespace()->GetName().ToString() << "::"
			<< name << "\", []() { return new_message<"
			<< GetNamespace()->GetName().ToString() << "::"
			<< name << ">(); });" << std::endl;
		stream << std::endl;
	}

	GenerateCppNamespaceClose(stream);

	return stream.str();
}

std::string Struct::GenerateTypescript() const
{
	std::stringstream stream;

	const std::string_view name = GetName().GetContents();

	{
		std::set<std::string> references;

		for (const Field &field : m_fields)
		{
			field.instance->AddTypescriptReferences(references);
		}

		for (const auto &ref : references)
			stream << ref << std::endl;

		if (!references.empty())
			stream << std::endl;
	}

	stream << "namespace ";
	{
		bool first = true;

		for (const Token &tok : GetNamespace()->GetName().GetTokens())
		{
			if (first)
				first = false;
			else
				stream << '.';

			stream << tok.GetContents();
		}
		stream << std::endl;
	}
	stream << '{' << std::endl;
	stream << "    export class " << name;
	if (m_is_message)
	{
		stream << " extends bbox.enc.MsgAnyPtr";
	}
	stream << std::endl;
	stream << "    {" << std::endl;
	stream << "        static TYPE: bbox.enc.Type = bbox.enc.TypeLibrary.simpleStructure(\""
		<< GetNamespace()->GetName().ToString() << "::" << name << "\", "
		<< name << ')' << std::endl;
	for (const Field &field : m_fields)
	{
		stream << "            .addMember(\""
			<< field.name.GetContents() << "\", "
			<< field.instance->GetTypescriptTypeLibraryConstructor() << ')' << std::endl;
	}
	stream << "            ;" << std::endl;
	stream << std::endl;
	if (!m_fields.empty())
	{
		for (const Field &field : m_fields)
			stream << "        public " << field.name.GetContents() << ": " << field.instance->GetTypescriptTypeName() << ';' << std::endl;
		stream << std::endl;
	}
	stream << "        constructor()" << std::endl;
	stream << "        {" << std::endl;
	if (m_is_message)
	{
		stream << "            super(" << name << ".TYPE);" << std::endl;
	}
	for (const Field &field : m_fields)
		stream << "            this." << field.name.GetContents() << " = " << field.instance->GetTypescriptDefaultValue() << ';' << std::endl;
	stream << "        }" << std::endl;
	stream << "    };" << std::endl;
	stream << '}' << std::endl;

	return stream.str();
}


} // namespace bbox::enc::compiler
} // namespace bbox::enc
} // namespace bbox
