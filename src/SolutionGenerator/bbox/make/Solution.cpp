/**
* @file
*
* Implementation file for the bbox::make::Soltuion class.
*/

#include <bbox/make/Solution.h>

#include <bbox/Exception.h>
#include <bbox/FileUtils.h>
#include <bbox/Format.h>
#include <bbox/LuaState.h>

#include <lauxlib.h>

namespace bbox {
    namespace make {

        Solution::Solution(const std::string &base_folder, const std::string &platform)
            : m_base_folder(base_folder)
            , m_platform(platform)
            , m_file_name()
        {
        }

        Solution::~Solution()
        {
        }

        std::vector<const Project *>
        Solution::GetProjects() const
        {
            std::vector<const Project *> result;

            result.reserve(m_projects.size());

            for (const auto &entry : m_projects)
            {
                result.push_back(entry.second.get());
            }

            return result;
        }

        const Project *
        Solution::GetProject(const std::string &name) const
        {
            auto it = m_projects.find(name);

            if (it == m_projects.end())
            {
                throw Exception(Format("Invalid project name \"%s\"", name));
            }

            return it->second.get();
        }

        void
        Solution::ParseMakeFiles()
        {
            // Run all of the files

            ParseFolder(m_base_folder, "");

            // Perform the integrity checks
            // after everything has been read in

            CheckReferences();
        }

        void
        Solution::ParseFolder(const std::string &path, const std::string &relative)
        {
            // First, search all the normal files for ".bbmake" files

            std::list<bbox::FileInfo> files =
                bbox::FileUtils::ListFolderOrThrow(
                    bbox::Format("%s\\*.bbmake", path),
                    bbox::FileInfo::RegularFile);

            bool got_file = false;

            for (const bbox::FileInfo &file : files)
            {
                // This is a ".bbmake" file - it needs processing
                // if we've not already done one

                if (got_file)
                    throw Exception(Format("Duplicate .bbmake files in folder \"%s\"", relative));

                got_file = true;

                // Work out the relative path name

                std::string sub_relative;
                sub_relative.reserve(relative.size() + 2 + file.GetName().size());
                sub_relative.append(relative);
                if (!sub_relative.empty())
                    sub_relative.append("/");
                sub_relative.append(file.GetName());

                // Read in the contents

                std::string bbmake_contents = bbox::FileUtils::ReadTextFileOrThrow(file.GetFullPath());

                // Create a state to run the file in

                LuaState lua_state;
                lua_state.LoadStandardLibraries();

                lua_state.RegisterGlobalFunction(
                    "platform",
                    boost::bind(
                        &Solution::LuaFunc_Platform,
                        this,
                        _1));

                if (relative.empty())
                {
                    // This is the top-level folder - we expect
                    // a solution to be generated

                    bool called = false;

                    lua_state.RegisterGlobalFunction(
                        "solution",
                        boost::bind(
                            &Solution::LuaFunc_Solution,
                            this,
                            _1,
                            path,
                            relative,
                            boost::ref(called)));

                    // Save the file name

                    m_file_name = file.GetName();

                    // Run the file

                    lua_state.CompileTextOrThrow(sub_relative, bbmake_contents);
                    lua_state.PCallOrThrow(0, 0);

                    // We expect that the function will have been called

                    if (!called)
                        throw Exception("Solution was not created");
                }
                else
                {
                    // In lower folder we expect projects

                    bool called = false;

                    lua_state.RegisterGlobalFunction(
                        "project",
                        boost::bind(
                            &Solution::LuaFunc_Project,
                            this,
                            _1,
                            path,
                            relative,
                            file.GetName(),
                            boost::ref(called)));

                    // Run the file

                    lua_state.CompileTextOrThrow(sub_relative, bbmake_contents);
                    lua_state.PCallOrThrow(0, 0);

                    // We don't nessesarily expect
					// a project to be created, because some
					// projects are only created for some platforms.
                }
            }

            // Now, search through the sub-folders
            // if we're the top level folder or if we
            // haven't found a make file in this folder.

            if (relative.empty() || !got_file)
            {
                std::list<bbox::FileInfo> folders =
                    bbox::FileUtils::ListFolderOrThrow(
                        bbox::Format("%s\\*", path),
                        bbox::FileInfo::Folder);

                for (const bbox::FileInfo &folder : folders)
                {
                    // Work out the relative naeme

                    std::string sub_relative;
                    sub_relative.reserve(relative.size() + 2 + folder.GetName().size());
                    sub_relative.append(relative);
                    if (!sub_relative.empty())
                        sub_relative.append("/");
                    sub_relative.append(folder.GetName());

                    ParseFolder(
                        folder.GetFullPath(),
                        CreateRelativeName(relative, folder.GetName()));
                }
            }
        }

        std::string
        Solution::CreateRelativeName(const std::string &relative, const std::string &name)
        {
            std::string result;
            result.reserve(relative.size() + 2 + name.size());
            result.append(relative);
            if (!result.empty())
                result.append(FileUtils::FOLDER_SEPERATOR);
            result.append(name);

            return result;
        }

        void
        Solution::CheckReferences()
        {
            for (const Project *project_ptr : GetProjects())
            {
                for (const std::string &ref : project_ptr->GetReferences())
                {
                    auto it = m_projects.find(ref);

                    if (it == m_projects.end())
                    {
                        throw Exception(Format(
                            "Undefined project \"%s\" referenced from project \"%s\"",
                            ref,
                            project_ptr->GetName()));
                    }
                }

                for (const std::string &ref : project_ptr->GetToolReferences())
                {
                    auto it = m_projects.find(ref);

                    if (it == m_projects.end())
                    {
                        throw Exception(Format(
                            "Undefined project \"%s\" referenced as tool from project \"%s\"",
                            ref,
                            project_ptr->GetName()));
                    }
                }
            }
        }

        int
        Solution::LuaFunc_Solution(LuaState &state,
                                   const std::string &path,
                                   const std::string &relative_path,
                                   bool &called)
        {
            state.ArgumentCountExactlyOrThrow(1);
            std::string name = state.ArgumentToStringOrThrow(1);

            if (called)
                throw Exception("Already created solution");

            called = true;
            m_name = name;

            if (m_file_name != (name + ".bbmake"))
                throw Exception("Solution name doesn't match file name");

            std::cout << "Creating SOLUTION " << name << " in folder \"" << relative_path << "\" (" << path << ")" << std::endl;

            return 0;
        }

        int
        Solution::LuaFunc_Platform(LuaState &state)
        {
            state.ArgumentCountExactlyOrThrow(0);

            lua_State *L = state.GetState();

            lua_pushstring(L, m_platform.c_str());

            return 1;
        }

        int
        Solution::LuaFunc_Project(LuaState &state,
                                  const std::string &path,
                                  const std::string &relative_path,
                                  const std::string &file_name,
                                  bool &called)
        {
            state.ArgumentCountExactlyOrThrow(2);
            std::string name = state.ArgumentToStringOrThrow(1);
            std::string type_str = state.ArgumentToStringOrThrow(2);

            if (called)
                throw Exception("Already created project");

            called = true; 

            if (file_name != (name + ".bbmake"))
                throw Exception("Project name doesn't match file name");

            if (m_projects.find(name) != m_projects.end())
                throw Exception("Duplicate project name");

            ProjectType type;
            if (!ProjectType::FromString(type_str, type))
                throw Exception(Format("Invalid project type \"%s\"", type_str));

            std::cout << "Creating PROJECT " << name << " in folder \"" << relative_path << "\" (" << path << ") type " << type.ToString() << std::endl;

            Project *project_ptr = new Project(*this, name, type, path, relative_path, file_name);

            m_projects[name] = std::unique_ptr<Project>(project_ptr);

            // Also register the required functions
            // that projects can use

            state.RegisterGlobalFunction(
                "reference",
                boost::bind(
                    &Solution::LuaFunc_Reference,
                    this,
                    _1,
                    project_ptr,
                    false));

            state.RegisterGlobalFunction(
                "tool_reference",
                boost::bind(
                    &Solution::LuaFunc_Reference,
                    this,
                    _1,
                    project_ptr,
                    true));

            state.RegisterGlobalFunction(
                "sources",
                boost::bind(
                    &Solution::LuaFunc_File_List,
                    this,
                    _1,
                    project_ptr,
                    SOURCE));

            state.RegisterGlobalFunction(
                "headers",
                boost::bind(
                    &Solution::LuaFunc_File_List,
                    this,
                    _1,
                    project_ptr,
                    HEADER));

            state.RegisterGlobalFunction(
                "resources",
                boost::bind(
                    &Solution::LuaFunc_File_List,
                    this,
                    _1,
                    project_ptr,
                    RESOURCE));

            state.RegisterGlobalFunction(
                "custom_build",
                boost::bind(
                    &Solution::LuaFunc_Custom_Build,
                    this,
                    _1,
                    project_ptr));

            state.RegisterGlobalFunction(
                "find",
                boost::bind(
                    &Solution::LuaFunc_Find,
                    this,
                    _1,
                    project_ptr,
                    false));

            state.RegisterGlobalFunction(
                "find_recursive",
                boost::bind(
                    &Solution::LuaFunc_Find,
                    this,
                    _1,
                    project_ptr,
                    true));

            state.RegisterGlobalFunction(
                "include",
                boost::bind(
                    &Solution::LuaFunc_Include,
                    this,
                    _1,
                    project_ptr));

            state.RegisterGlobalFunction(
                "define",
                boost::bind(
                    &Solution::LuaFunc_Define,
                    this,
                    _1,
                    project_ptr));

            state.RegisterGlobalFunction(
                "define_if",
                boost::bind(
                    &Solution::LuaFunc_DefineIf,
                    this,
                    _1,
                    project_ptr));

            state.RegisterGlobalFunction(
                "library_reference",
                boost::bind(
                    &Solution::LuaFunc_LibraryReference,
                    this,
                    _1,
                    project_ptr));

            state.RegisterGlobalFunction(
                "link_all_objects",
                boost::bind(
                    &Solution::LuaFunc_LinkAllObjects,
                    this,
                    _1,
                    project_ptr));

            return 0;
        }

        int
        Solution::LuaFunc_Reference(LuaState &state, Project *project_ptr, bool tool_ref)
        {
            state.ArgumentCountExactlyOrThrow(1);
            std::string reference = state.ArgumentToStringOrThrow(1);

            if (tool_ref)
                project_ptr->AddToolReference(reference);
            else
                project_ptr->AddReference(reference);

            return 0;
        }

        int
        Solution::LuaFunc_File_List(LuaState &state, Project *project_ptr, E_FILE_LIST_TYPE type)
        {
			lua_State *L = state.GetState();

			int num_args = lua_gettop(L);

			auto add_file = [project_ptr, type](const std::string &entry_str)
			{
				switch (type)
				{
				case SOURCE:    project_ptr->AddSource(entry_str); break;
				case HEADER:    project_ptr->AddHeader(entry_str); break;
				case RESOURCE:  project_ptr->AddResource(entry_str); break;
				}
			};

			for (int i = 1; i <= num_args; ++i)
			{
				if (lua_isstring(L, i))
				{
					add_file(lua_tostring(L, i));
				}
				else if (lua_istable(L, i))
				{
					lua_pushnil(L);
					while (lua_next(L, i) != 0)
					{
						if (!lua_isstring(L, num_args + 2))
							return luaL_error(L, "Contents of table arguments must be strings");

						add_file(lua_tostring(L, num_args + 2));

						lua_pop(L, 1);
					}
				}
				else
				{
					return luaL_error(L, "Arguments must be string or table");
				}
			}

            return 0;
        }

        int
        Solution::LuaFunc_Custom_Build(LuaState &state, Project *project_ptr)
        {
            state.ArgumentCountExactlyOrThrow(4);

            lua_State *L = state.GetState();

            if (!lua_isstring(L, 1))
                return luaL_error(L, "First argument must be a string");
            if (!lua_istable(L, 2))
                return luaL_error(L, "Second argument must be a table");
            if (!lua_istable(L, 3))
                return luaL_error(L, "Third argument must be a table");
            if (!lua_isstring(L, 4))
                return luaL_error(L, "Fourth argument must be a string");

            std::string tool_str = lua_tostring(L, 1);
            std::string extra_args_str = lua_tostring(L, 4);

            std::set<std::string> inputs;
            std::set<std::string> outputs;

            lua_pushnil(L);
            while (lua_next(L, 2))
            {
                if (!lua_isstring(L, 6))
                    return luaL_error(L, "Contents of second argument must be strings");

                std::string entry_str = lua_tostring(L, 6);
                inputs.insert(entry_str);

                lua_pop(L, 1);
            }

            lua_pushnil(L);
            while (lua_next(L, 3))
            {
                if (!lua_isstring(L, 6))
                    return luaL_error(L, "Contents of second argument must be strings");

                std::string entry_str = lua_tostring(L, 6);
                outputs.insert(entry_str);

                lua_pop(L, 1);
            }

            project_ptr->AddCustomBuild(tool_str, inputs, outputs, extra_args_str);

            return 0;
        }

        int
        Solution::LuaFunc_Find(LuaState &state, Project *project_ptr, bool recursive)
        {
			lua_State *L = state.GetState();

			int num_args = lua_gettop(L);

			// Create the table - which will be at index num_args + 1

            lua_newtable(L);

			unsigned next_index = 1;

			// Search for each argument

			for (int i = 1; i <= num_args; ++i)
			{
				std::string argument = state.ArgumentToStringOrThrow(i);

				// Replace all the forward-slashes with back slashes
				for (size_t i = 0; i < argument.size(); ++i)
				{
					if (argument[i] == '/')
						argument[i] = '\\';
				}

				// Split into folder and search string
				std::string folder_relative;
				std::string search;
				{
					size_t pos = argument.rfind('\\');
					if (pos == std::string::npos)
					{
						// No folder
						search = argument;
					}
					else
					{
						folder_relative = argument.substr(0, pos);
						search = argument.substr(pos + 1);
					}
				}

				// Now work out the full folder path

				std::string folder_path;
				{
					folder_path.reserve(project_ptr->GetPath().size() + folder_relative.size() + 2);
					folder_path.append(project_ptr->GetPath());
					if (!folder_relative.empty())
					{
						folder_path.push_back('\\');
						folder_path.append(folder_relative);
					}
					folder_path.push_back('\\');
				}

				// Now - run the search on the top level folder

				Error err = LuaFunc_DoFind(state, num_args + 1, next_index, folder_path, folder_relative, search, recursive);

				if (err)
				{
					return luaL_error(L, "Error finding files: %s", err.ToString().c_str());
				}
			}

            // Return the table

            return 1;
        }

        Error
        Solution::LuaFunc_DoFind(LuaState &state, int table_index, unsigned &next_index, const std::string &folder_path, const std::string &folder_relative, const std::string &search, bool recursive)
        {
            lua_State *L = state.GetState();

            // First, find all the regular files that
            // match the search

            std::list<bbox::FileInfo> files;
            Error error;
            
            error = bbox::FileUtils::ListFolder(
                    bbox::Format("%s%s", folder_path, search),
                    bbox::FileInfo::RegularFile,
                    files);
            if (error)
                return error;

            for (const bbox::FileInfo &file : files)
            {
                std::string file_relative = folder_relative;
                if (!file_relative.empty())
                    file_relative.push_back('\\');
                file_relative.append(file.GetName());

                lua_pushunsigned(L, next_index++);
                lua_pushstring(L, file_relative.c_str());
                lua_settable(L, table_index);
            }

            // Then, if recusive also perform the search
            // on all sub-folders

            if (recursive)
            {
                std::list<bbox::FileInfo> subfolders;
                error = bbox::FileUtils::ListFolder(
                        bbox::Format("%s*", folder_path),
                        bbox::FileInfo::Folder,
                        subfolders);

                if (error)
                    return error;

                for (const bbox::FileInfo &subfolder : subfolders)
                {
                    std::string subfolder_path = folder_path;
                    subfolder_path.append(subfolder.GetName());
                    subfolder_path.push_back('\\');

                    std::string subfolder_relative = folder_relative;
                    if (!subfolder_relative.empty())
                        subfolder_relative.push_back('\\');
                    subfolder_relative.append(subfolder.GetName());

                    LuaFunc_DoFind(state, table_index, next_index, subfolder_path, subfolder_relative, search, recursive);
                }
            }

            return Error();
        }

        int
        Solution::LuaFunc_Include(LuaState &state, Project *project_ptr)
        {
            state.ArgumentCountExactlyOrThrow(1);
            std::string argument = state.ArgumentToStringOrThrow(1);

            project_ptr->AddInclude(argument);

            return 0;
        }

        int
        Solution::LuaFunc_Define(LuaState &state, Project *project_ptr)
        {
            lua_State *L = state.GetState();

            int arguments = lua_gettop(L);

            if (arguments == 1)
            {
                std::string define = state.ArgumentToStringOrThrow(1);

                project_ptr->AddDefine(define, "", "");
            }
            else if (arguments == 2)
            {
                std::string define = state.ArgumentToStringOrThrow(1);
                std::string value = state.ArgumentToStringOrThrow(2);

                project_ptr->AddDefine(define, value, "");
            }
            else
            {
                return luaL_error(L, "define function expects 1 or 2 arguments, not %d", arguments);
            }

            return 0;
        }

        int
        Solution::LuaFunc_DefineIf(LuaState &state, Project *project_ptr)
        {
            lua_State *L = state.GetState();

            int arguments = lua_gettop(L);

            if (arguments == 2)
            {
                std::string condition = state.ArgumentToStringOrThrow(1);
                std::string define = state.ArgumentToStringOrThrow(2);

                project_ptr->AddDefine(define, "", condition);
            }
            else
            {
                return luaL_error(L, "define_if function expects 2 arguments, not %d", arguments);
            }

            return 0;
        }

        int
        Solution::LuaFunc_LibraryReference(LuaState &state, Project *project_ptr)
        {
            state.ArgumentCountExactlyOrThrow(1);
            std::string argument = state.ArgumentToStringOrThrow(1);

            project_ptr->AddAdditionalDependency(argument);

            return 0;
        }

        int
        Solution::LuaFunc_LinkAllObjects(LuaState &state, Project *project_ptr)
        {
            state.ArgumentCountExactlyOrThrow(0);

            project_ptr->SetLinkAllObjects();

            return 0;
        }

    } // namespace bbox::make
} // namespace bbox
