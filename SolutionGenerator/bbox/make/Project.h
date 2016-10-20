/**
* @file
*
* Header file for the bbox::make::Project class.
*/

#ifndef __BBOX__MAKE__PROJECT_H__
#define __BBOX__MAKE__PROJECT_H__

#pragma once

#include <string>
#include <set>
#include <map>
#include <list>

#include <bbox/make/ProjectType.h>

namespace bbox {

    namespace make {

        // Forward declaration
        class Solution;

        /**
         * A project in a solution.
         */
        class Project
        {
        public:

            struct CustomBuild
            {
                std::string tool;
                std::set<std::string> inputs;
                std::set<std::string> outputs;
                std::string extra_args;

                CustomBuild(std::string _tool, std::set<std::string> _inputs, std::set<std::string> _outputs, std::string _extra_args)
                    : tool(std::move(_tool))
                    , inputs(std::move(_inputs))
                    , outputs(std::move(_outputs))
                    , extra_args(std::move(_extra_args))
                {
                }
            };

            struct Define
            {
                std::string value;
                std::string condition;

                Define(const std::string &_value, const std::string &_condition)
                    : value(_value), condition(_condition)
                {
                }
            };

            Project(Solution &solution, std::string name, ProjectType type, std::string path, std::string relative_path, std::string file_name);
            ~Project();

            const Solution &GetSolution() const         { return m_solution; }
            Solution &GetSolution()                     { return m_solution; }

            ProjectType GetType() const                 { return m_type; }
            const std::string &GetName() const          { return m_name; }
            const std::string &GetPath() const          { return m_path; }
            const std::string &GetRelativePath() const  { return m_relative_path; }
            const std::string &GetFileName() const      { return m_file_name; }

            const std::set<std::string> &GetReferences() const { return m_references; }
            const std::set<std::string> &GetToolReferences() const { return m_tool_references; }
            const std::set<std::string> &GetSources() const { return m_sources; }
            const std::set<std::string> &GetHeaders() const { return m_headers; }
            const std::set<std::string> &GetResources() const { return m_resources; }
            const std::set<std::string> &GetIncludes() const { return m_includes; }
            const std::map<std::string, Define> &GetDefines() const { return m_defines; }
            const std::set<std::string> &GetAdditionalDependencies() const { return m_additional_dependencies; }

            const std::list<CustomBuild> &GetCustomBuilds() const { return m_custom_builds; }

            bool IsLinkAllObjectsSet() const { return m_link_all_objects; }

            std::set<std::string> GetReferencesRecursive() const;
            std::set<std::string> GetSelfPlusReferencesRecursive() const;

            std::string CalculateReverseRelativePath() const;

            void AddReference(const std::string &reference);
            void AddToolReference(const std::string &tool_reference);
            void AddSource(const std::string &source);
            void AddHeader(const std::string &header);
            void AddResource(const std::string &resource);
            void AddInclude(const std::string &include);
            void AddDefine(const std::string &define, const std::string &value, const std::string &condition);
            void AddAdditionalDependency(const std::string &additional_dep);

            void AddCustomBuild(const std::string &tool, const std::set<std::string> &inputs, const std::set<std::string> &outputs, const std::string &extra_args);

            void SetLinkAllObjects() { m_link_all_objects = true; }

        private:
            Solution &m_solution;
            const ProjectType m_type;
            const std::string m_name;
            const std::string m_path;
            const std::string m_relative_path;
            const std::string m_file_name;
            std::set<std::string> m_references;
            std::set<std::string> m_tool_references;
            std::set<std::string> m_sources;
            std::set<std::string> m_headers;
            std::set<std::string> m_resources;
            std::set<std::string> m_includes;
            std::map<std::string, Define> m_defines;
            std::list<CustomBuild> m_custom_builds;
            std::set<std::string> m_additional_dependencies;
            bool m_link_all_objects;
        };

    } // namespace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__PROJECT_H__