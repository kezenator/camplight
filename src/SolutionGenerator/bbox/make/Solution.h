/**
* @file
*
* Header file for the bbox::make::Soltuion class.
*/

#ifndef __BBOX__MAKE__SOLUTION_H__
#define __BBOX__MAKE__SOLUTION_H__

#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include <bbox/Error.h>
#include <bbox/make/Project.h>

namespace bbox {

    // Forward declarations
    class LuaState;
    class Project;

    namespace make {

        /**
         * Represents the whole solution - a collection
         * of individual projects.
         */
        class Solution
        {
            // Not copyable
            Solution(const Solution &) = delete;
            void operator =(const Solution &) = delete;

        public:
            Solution(const std::string &base_folder, const std::string &platform);
            ~Solution();

            const std::string &GetPlatform() const      { return m_platform; }
            const std::string &GetBaseFolder() const    { return m_base_folder; }
            const std::string &GetFileName() const      { return m_file_name; }
            const std::string &GetName() const          { return m_name; }
            std::vector<const Project *> GetProjects() const;
            const Project *GetProject(const std::string &name) const;

            void ParseMakeFiles();

        private:

            void ParseFolder(const std::string &path, const std::string &relative);
            void CheckReferences();
            std::string CreateRelativeName(const std::string &relative, const std::string &name);

            enum E_FILE_LIST_TYPE
            {
                SOURCE,
                HEADER,
                RESOURCE
            };

            int LuaFunc_Solution(LuaState &state, const std::string &path, const std::string &relative_path, bool &called);
            int LuaFunc_Platform(LuaState &state);
            int LuaFunc_Project(LuaState &state, const std::string &path, const std::string &relative_path, const std::string &file_name, bool &called);
            int LuaFunc_Reference(LuaState &state, Project *project_ptr, bool tool_ref);
            int LuaFunc_File_List(LuaState &state, Project *project_ptr, E_FILE_LIST_TYPE type);
            int LuaFunc_Custom_Build(LuaState &state, Project *project_ptr);
            int LuaFunc_Find(LuaState &state, Project *project_ptr, bool recursive);
            int LuaFunc_Include(LuaState &state, Project *project_ptr);
            int LuaFunc_Define(LuaState &state, Project *project_ptr);
            int LuaFunc_DefineIf(LuaState &state, Project *project_ptr);
            int LuaFunc_LibraryReference(LuaState &state, Project *project_ptr);
            int LuaFunc_LinkAllObjects(LuaState &state, Project *project_ptr);

            Error LuaFunc_DoFind(LuaState &state, int table_index, unsigned &next_index, const std::string &folder_path, const std::string &folder_relative, const std::string &search, bool recursive);

            const std::string m_base_folder;
            const std::string m_platform;
            std::string m_file_name;
            std::string m_name;
            std::map<std::string, std::unique_ptr<Project>> m_projects;
        };

    } // namespace bbox::make
} // namespace bbox

#endif // __BBOX__MAKE__SOLUTION_H__