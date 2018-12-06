/**
* @file
*
* Implementation file for the bbox::make::Project class.
*/

#include <bbox/make/Project.h>
#include <bbox/make/Solution.h>
#include <bbox/FileUtils.h>

#include <bbox/Exception.h>
#include <bbox/Format.h>
#include <bbox/Assert.h>

namespace bbox {
    namespace make {

        Project::Project(Solution &solution, std::string name, ProjectType type, std::string path, std::string relative_path, std::string file_name)
            : m_solution(solution)
            , m_type(type)
            , m_name(std::move(name))
            , m_path(std::move(path))
            , m_relative_path(std::move(relative_path))
            , m_file_name(std::move(file_name))
            , m_link_all_objects(false)
        {
        }

        Project::~Project()
        {
        }

        std::set<std::string>
        Project::GetReferencesRecursive() const
        {
            std::set<std::string> result;
            std::set<std::string> to_check;

            to_check.insert(m_name);

            while (!to_check.empty())
            {
                std::string check_name = *to_check.begin();
                to_check.erase(check_name);

                const Project *check_proj = m_solution.GetProject(check_name);

                for (const std::string &ref : check_proj->GetReferences())
                {
                    // For all references for this project,
                    // if they're not already in the result then
                    // add them to both the result and the list to check

                    bool in_result = (result.find(ref) != result.end());

                    if (!in_result)
                    {
                        result.insert(ref);
                        to_check.insert(ref);
                    }
                }
            }

            return result;
        }

        std::set<std::string>
        Project::GetSelfPlusReferencesRecursive() const
        {
            std::set<std::string> result = GetReferencesRecursive();

            result.insert(m_name);

            return result;
        }

        std::string
        Project::CalculateReverseRelativePath() const
        {
            if (m_relative_path.empty())
                return ".";

            std::string result = "..";

            for (char ch : m_relative_path)
            {
                if ((ch == '/') || (ch == '\\'))
                {
                    result.append("/..");
                }
            }

            return result;
        }

        void
        Project::AddReference(const std::string &reference)
        {
            if (m_tool_references.find(reference) != m_tool_references.end())
                throw Exception(Format("Project reference \"%s\" already added as tool reference", reference));

            auto insert_result = m_references.insert(reference);

            if (!insert_result.second)
                throw Exception(Format("Duplicate project reference \"%s\"", reference));
        }

        void
        Project::AddToolReference(const std::string &tool_reference)
        {
            if (m_references.find(tool_reference) != m_references.end())
                throw Exception(Format("Project tool reference \"%s\" already added as reference", tool_reference));

            auto insert_result = m_tool_references.insert(tool_reference);

            if (!insert_result.second)
                throw Exception(Format("Duplicate project tool reference \"%s\"", tool_reference));
        }

        void
        Project::AddSource(const std::string &source)
        {
            m_sources.insert(FileUtils::ToUnixPath(source));
        }

        void
        Project::AddHeader(const std::string &header)
        {
            m_headers.insert(FileUtils::ToUnixPath(header));
        }

        void
        Project::AddRcFile(const std::string &rc_file)
        {
            m_rc_files.insert(FileUtils::ToUnixPath(rc_file));
        }

        void
        Project::AddResource(const std::string &resource)
        {
            m_resources.insert(FileUtils::ToUnixPath(resource));
        }

        void
        Project::AddInclude(const std::string &include)
        {
                auto insert_result = m_includes.insert(FileUtils::ToUnixPath(include));

                if (!insert_result.second)
                    throw Exception(Format("Duplicate project include \"%s\"", include));
        }

        void
        Project::AddDefine(const std::string &define, const std::string &value, const std::string &condition)
        {
            auto insert_result = m_defines.insert(std::make_pair(define, Define(value, condition)));

            if (!insert_result.second)
                throw Exception(Format("Duplicate project define \"%s\"", define));
        }

        void
        Project::AddCustomBuild(
            const std::string &tool,
            const std::set<std::string> &inputs,
            const std::set<std::string> &outputs,
            const std::set<std::string> &depedent_sources,
            const std::string &extra_args)
        {
            if (m_tool_references.find(tool) == m_tool_references.end())
                throw Exception(Format("Custom build tool \"%s\" is not referenced by project \"%s\"", tool, m_name));

            m_custom_builds.emplace_back(tool, inputs, outputs, depedent_sources, extra_args);
        }

        void
        Project::AddAdditionalDependency(const std::string &additional_dep)
        {
            auto insert_result = m_additional_dependencies.insert(additional_dep);

            if (!insert_result.second)
                throw Exception(Format("Duplicate project dependency \"%s\"", additional_dep));
        }

    } // namespace bbox::make
} // namespace bbox
