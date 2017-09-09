/**
* @file
*
* Implementation file for the bbox::make::vs::VisualStudioGenerator class.
*/

#include <bbox/make/vs/VisualStudioGenerator.h>
#include <bbox/make/vs/DodgyXmlGenerator.h>
#include <bbox/make/Solution.h>
#include <bbox/make/Project.h>

#include <bbox/Assert.h>
#include <bbox/Format.h>
#include <bbox/Uuid.h>
#include <bbox/FileUtils.h>

#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

namespace bbox {
    namespace make {
        namespace vs {

            VisualStudioGenerator::VisualStudioGenerator(const Solution &solution)
                : Generator(solution)
            {
            }

            VisualStudioGenerator::~VisualStudioGenerator()
            {
            }

            void
            VisualStudioGenerator::UpdateFiles()
            {
                // First, generate a unique GUID for
                // each project - these are used
                // throughout the solution and project
                // files

                GenerateProjectGuids();

				// Reset the ISS port base

				m_iis_port = 54099;

                // Generate the overall solution file

                GenerateSolutionFile();

                // Generate the project files

                for (auto project_ptr : GetSolution().GetProjects())
                {
                    GenerateProjectFiles(project_ptr);
                }
            }

            void
            VisualStudioGenerator::GenerateProjectGuids()
            {
                m_project_guids.clear();

                // First, we need to generate a name-space UUID.
                // I've just generated a random Type 1 (time based)
                // UUID to use as the namespace

                Uuid namespace_uuid =
                { 0x1C, 0x2E, 0x9E, 0x30,
                  0x96, 0x89,
                  0x11, 0xE3,
                  0xA5, 0xE2,
                  0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };

                std::vector<const Project *> projects = GetSolution().GetProjects();
                                   
                for (auto project_ptr : projects)
                {
                    if (project_ptr->GetType() != ProjectType::TypeScriptLibrary)
                    {
                        const std::string &name = project_ptr->GetName();

                        std::string project_guid;
                        {
                            Uuid project_uuid;
                            project_uuid.GenerateNamed(namespace_uuid, name);

                            project_guid = project_uuid.ToString();
                        }

                        m_project_guids[name] = project_guid;

                        std::cout << "Project "
                            << name
                            << " =>"
                            << project_guid
                            << std::endl;
                    }
                }
            }

            std::string
            VisualStudioGenerator::CalcPathToSolution(const Project *project_ptr)
            {
                std::string path_to_solution_folder = "..";
                {
                    struct is_slash
                    {
                        bool operator()(char c)
                        {
                            return c == '\\';
                        }
                    };

                    const std::string rel_path =
                        FileUtils::ToWindowsPath(project_ptr->GetRelativePath());

                    size_t num_slash = std::count_if(rel_path.begin(), rel_path.end(), is_slash());

                    for (size_t i = 0; i < num_slash; i++)
                    {
                        path_to_solution_folder.append("\\..");
                    }
                }

                return path_to_solution_folder;
            }

            void
            VisualStudioGenerator::GenerateSolutionFile()
            {
                std::stringstream stream;

                // Initial headers

                stream << "Microsoft Visual Studio Solution File, Format Version 12.00" << std::endl;
                stream << "# Visual Studio 14" << std::endl;
                stream << "VisualStudioVersion = 14.0.23107.0" << std::endl;
                stream << "MinimumVisualStudioVersion = 10.0.40219.1" << std::endl;

                // References to each project

                for (auto project_ptr : GetSolution().GetProjects())
                {
                    if (project_ptr->GetType() == ProjectType::TypeScriptLibrary)
                        continue;

                    const std::string &project_name = project_ptr->GetName();
                    const std::string &project_guid = m_project_guids[project_name];

                    std::string type_guid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
                    std::string extension = ".vcxproj";

                    if (project_ptr->GetType() == ProjectType::TypeScriptApplication)
                    {
                        type_guid = "{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}";
                        extension = ".csproj";
                    }

                    stream
                        << "Project(\""
                        << type_guid
                        << "\") = \""
                        << project_name
                        << "\", \""
                        << FileUtils::ToWindowsPath(project_ptr->GetRelativePath())
                        << "\\"
                        << project_name
                        << extension
                        << "\", \""
                        << project_guid
                        << "\""
                        << std::endl;

                    const std::set<std::string> &tool_references = project_ptr->GetToolReferences();

                    if (!tool_references.empty())
                    {
                        stream
                            << "\tProjectSection(ProjectDependencies) = postProject"
                            << std::endl;

                        for (const std::string &tool_ref : tool_references)
                        {
                            stream
                                << "\t\t"
                                << m_project_guids[tool_ref]
                                << " = "
                                << m_project_guids[tool_ref]
                                << std::endl;
                        }

                        stream
                            << "\tEndProjectSection"
                            << std::endl;
                    }

                    stream << "EndProject" << std::endl;
                }

                // Start the global section

                stream << "Global" << std::endl;
                stream << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << std::endl;   
                stream << "\t\tDebug|Any CPU = Debug|Any CPU" << std::endl;
                stream << "\t\tDebug|Win32 = Debug|Win32" << std::endl;
                stream << "\t\tDebug|x64 = Debug|x64" << std::endl;
                stream << "\t\tRelease|Any CPU = Release|Any CPU" << std::endl;
                stream << "\t\tRelease|Win32 = Release|Win32" << std::endl;
                stream << "\t\tRelease|x64 = Release|x64" << std::endl;
                stream << "\tEndGlobalSection" << std::endl;
                stream << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution" << std::endl;

                // Print out each project

                for (auto project_ptr : GetSolution().GetProjects())
                {
                    if (project_ptr->GetType() == ProjectType::TypeScriptLibrary)
                        continue;

                    const std::string &project_name = project_ptr->GetName();
                    const std::string &project_guid = m_project_guids[project_name];

                    for (std::string mode : { "Debug", "Release" })
                    {
                        for (std::string platform : { "Any CPU", "Win32", "x64" })
                        {
                            std::string active_platform = platform;
                            bool has_build = true;

                            if ((platform == "Any CPU")
                                && (project_ptr->GetType() != ProjectType::TypeScriptApplication))
                            {
                                // For C++ projects, chose Win32
                                // as the active config in the
                                // "Any CPU" platform, and don't add
                                // a build entry

                                active_platform = "Win32";
                                has_build = false;
                            }
                            else if (project_ptr->GetType() == ProjectType::TypeScriptApplication)
                            {
                                // For TypeScriptApplication projects,
                                // the active platform is always
                                // "Any CPU".

                                active_platform = "Any CPU";
                            }

                            stream
                                << "\t\t"
                                << project_guid
                                << "."
                                << mode
                                << "|"
                                << platform
                                << ".ActiveCfg = "
                                << mode
                                << "|"
                                << active_platform
                                << std::endl;

                            if (has_build)
                            {
                                stream
                                    << "\t\t"
                                    << project_guid
                                    << "."
                                    << mode
                                    << "|"
                                    << platform
                                    << ".Build.0 = "
                                    << mode
                                    << "|"
                                    << active_platform
                                    << std::endl;
                            }
                        }
                    }
                }

                // Final closing sections

                stream << "\tEndGlobalSection" << std::endl;
                stream << "\tGlobalSection(SolutionProperties) = preSolution" << std::endl;
                stream << "\t\tHideSolutionNode = FALSE" << std::endl;
                stream << "\tEndGlobalSection" << std::endl;
                stream << "EndGlobal" << std::endl;

                // Convert to DOS line endings

                std::string contents = stream.str();

                boost::replace_all(contents, "\n", "\r\n");

                WriteFile(GetSolution().GetName() + ".sln", contents);
            }

            void
            VisualStudioGenerator::GenerateProjectFiles(const Project *project_ptr)
            {
                if (project_ptr->GetType() == ProjectType::TypeScriptLibrary)
                {
                    // Don't generate any files - these files just
                    // get included in any application that uses them
                }
                else if (project_ptr->GetType() == ProjectType::TypeScriptApplication)
                {
                    // Generate typescript project files

                    GenerateTypescriptCsproj(project_ptr);
                    GenerateTypescriptWebConfigs(project_ptr);
                }
                else
                {
                    // Generate C++ project files

                    GenerateProjectVcxproj(project_ptr);
                    GenerateProjectFilters(project_ptr);
                }
            }

            void
            VisualStudioGenerator::GenerateProjectVcxproj(const Project *project_ptr)
            {
                const std::string &name = project_ptr->GetName();
                const std::string &relative_path = project_ptr->GetRelativePath();
                const std::string &guid = m_project_guids[name];

                DodgyXmlGenerator doc;
                {
                    // Top level project elemennt

                    DodgyXmlGenerator::Element tle(doc, "Project");

                    doc.SetAttribute("DefaultTargets", "Build");
                    doc.SetAttribute("ToolsVersion", "14.0");
                    doc.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

                    // ProjectConfigurations item group

                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        doc.SetAttribute("Label", "ProjectConfigurations");

                        for (std::string mode : { "Debug", "Release" })
                        {
                            for (std::string platform : { "Win32", "x64" })
                            {
                                DodgyXmlGenerator::Element pc(doc, "ProjectConfiguration");

                                doc.SetAttribute("Include", mode + "|" + platform);

                                {
                                    DodgyXmlGenerator::Element config_element(doc, "Configuration");

                                    doc.SetText(mode);
                                }

                                {
                                    DodgyXmlGenerator::Element platform_element(doc, "Platform");

                                    doc.SetText(platform);
                                }
                            }
                        }
                    }

                    // Global property group

                    {
                        DodgyXmlGenerator::Element property_group(doc, "PropertyGroup");

                        doc.SetAttribute("Label", "Globals");

                        {
                            DodgyXmlGenerator::Element property_group(doc, "ProjectGuid");
                            doc.SetText(guid);
                        }

                        {
                            DodgyXmlGenerator::Element property_group(doc, "Keyword");
                            doc.SetText("Win32Proj");
                        }

                        {
                            DodgyXmlGenerator::Element property_group(doc, "RootNamespace");
                            doc.SetText(name);
                        }
                    }

                    // Import default settings

                    {
                        DodgyXmlGenerator::Element import(doc, "Import");

                        doc.SetAttribute(
                            "Project",
                            "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
                    }

                    // Import group

                    for (std::string mode : { "Debug", "Release" })
                    {
                        for (std::string platform : { "Win32", "x64" })
                        {
                            DodgyXmlGenerator::Element property_group(doc, "PropertyGroup");

                            doc.SetAttribute("Condition",
                                             std::string("'$(Configuration)|$(Platform)'=='")
                                             + mode + "|" + platform + "'");
                            doc.SetAttribute("Label", "Configuration");

                            const char *configuration_type = nullptr;
                            switch (project_ptr->GetType().GetValue())
                            {
                            case ProjectType::Application:      configuration_type = "Application"; break;
                            case ProjectType::StaticLibrary:    configuration_type = "StaticLibrary"; break;
                            case ProjectType::UnitTests:        configuration_type = "DynamicLibrary"; break;
                            default:                            BBOX_ASSERT(false);
                            }

                            doc.SetTextElement(
                                "ConfigurationType",
                                configuration_type);
                            doc.SetTextElement(
                                "UseDebugLibraries",
                                (mode == "Debug") ? "true" : "false");
                            doc.SetTextElement(
                                "PlatformToolset",
                                "v140");

                            if (mode == "Release")
                            {
                                doc.SetTextElement(
                                    "WholeProgramOptimization",
                                    "true");
                            }

                            doc.SetTextElement(
                                "CharacterSet",
                                "Unicode");

                            if (project_ptr->GetType() == ProjectType::UnitTests)
                            {
                                doc.SetTextElement(
                                    "UseOfMfc",
                                    "false");
                            }
                        }
                    }

                    // More default settings

                    {
                        DodgyXmlGenerator::Element import(doc, "Import");

                        doc.SetAttribute(
                            "Project",
                            "$(VCTargetsPath)\\Microsoft.Cpp.props");
                    }

                    {
                        DodgyXmlGenerator::Element import_group(doc, "ImportGroup");

                        doc.SetAttribute("Label", "ExtensionSettings");
                    }

                    // Property sheets import group

                    for (std::string mode : { "Debug", "Release" })
                    {
                        for (std::string platform : { "Win32", "x64" })
                        {
                            DodgyXmlGenerator::Element import_group(doc, "ImportGroup");

                            doc.SetAttribute("Label", "PropertySheets");
                            doc.SetAttribute("Condition",
                                             std::string("'$(Configuration)|$(Platform)'=='")
                                                + mode + "|" + platform + "'");

                            DodgyXmlGenerator::Element import(doc, "Import");

                            doc.SetAttribute("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
                            doc.SetAttribute("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
                            doc.SetAttribute("Label", "LocalAppDataPlatform");
                        }
                    }

                    // User macros

                    {
                        DodgyXmlGenerator::Element property_group(doc, "PropertyGroup");

                        doc.SetAttribute("Label", "UserMacros");
                    }

                    // Incremental linking (only for applications)

                    if ((project_ptr->GetType() == ProjectType::Application)
                        || (project_ptr->GetType() == ProjectType::UnitTests))
                    {
                        for (std::string mode : { "Debug", "Release" })
                        {
                            for (std::string platform : { "Win32", "x64" })
                            {
                                DodgyXmlGenerator::Element property_group(doc, "PropertyGroup");
                                doc.SetAttribute("Condition",
                                                 std::string("'$(Configuration)|$(Platform)'=='")
                                                    + mode + "|" + platform + "'");

                                doc.SetTextElement(
                                    "LinkIncremental",
                                    "true");
                            }
                        }
                    }

                    // Work out the path to the solution folder which is required
                    // by both the include settings and the project references.

                    const std::string path_to_solution_folder = CalcPathToSolution(project_ptr);

                    // Work out the additional include directories
                    // to print in the main project settings below

                    std::string additional_include_dirs;
                    {
                        for (const std::string &raw_include : project_ptr->GetIncludes())
                        {
                            if (!additional_include_dirs.empty())
                                additional_include_dirs.push_back(';');

                            std::string include = FileUtils::ToWindowsPath(raw_include);

                            additional_include_dirs.append(
                                Format(
                                    "$(ProjectDir)%s%s",
                                    (include == ".") ? "" : "\\",
                                    (include == ".") ? "" : include));
                        }

                        std::set<std::string> refs = project_ptr->GetReferencesRecursive();

                        for (const std::string &ref : refs)
                        {
                            const Project *ref_project_ptr = GetSolution().GetProject(ref);

                            for (const std::string &raw_include : ref_project_ptr->GetIncludes())
                            {
                                if (!additional_include_dirs.empty())
                                    additional_include_dirs.push_back(';');

                                std::string include = FileUtils::ToWindowsPath(raw_include);

                                additional_include_dirs.append(
                                    Format(
                                        "$(ProjectDir)\\%s\\%s%s%s",
                                        path_to_solution_folder,
                                        FileUtils::ToWindowsPath(ref_project_ptr->GetRelativePath()),
                                        (include == ".") ? "" : "\\",
                                        (include == ".") ? "" : include));
                            }
                        }

                        if (project_ptr->GetType() == ProjectType::UnitTests)
                        {
                            if (!additional_include_dirs.empty())
                                additional_include_dirs.push_back(';');

                            additional_include_dirs.append("$(VCInstallDir)UnitTest\\include");
                        }
                    }

                    // Main project settings

                    for (std::string mode : { "Debug", "Release" })
                    {
                        for (std::string platform : {"Win32", "x64"})
                        {
                            // Calculate the pre-processor defines for this mode

                            std::string preprocessor_defines = "WIN32;NTDDI_VERSION=0x06030000;_WIN32_WINNT=0x0602;WINVER=0x0602;_CONSOLE;_LIB;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS";

                            if (mode == "Debug")
                                preprocessor_defines += ";_DEBUG";
                            else
                                preprocessor_defines += ";NDEBUG";

                            for (const std::string &ref : project_ptr->GetSelfPlusReferencesRecursive())
                            {
                                const Project *ref_ptr = GetSolution().GetProject(ref);

                                for (const auto &entry : ref_ptr->GetDefines())
                                {
                                    const std::string &define = entry.first;
                                    const Project::Define &details = entry.second;

                                    if (details.condition.empty()
                                        || boost::iequals(details.condition, mode))
                                    {
                                        preprocessor_defines.push_back(';');
                                        preprocessor_defines.append(define);

                                        if (!details.value.empty())
                                        {
                                            preprocessor_defines.append("=");
                                            preprocessor_defines.append(details.value);
                                        }
                                    }
                                }
                            }

                            preprocessor_defines += ";%(PreprocessorDefinitions)";

                            // Generate the output

                            DodgyXmlGenerator::Element item_group(doc, "ItemDefinitionGroup");

                            doc.SetAttribute(
                                "Condition",
                                Format("'$(Configuration)|$(Platform)'=='%s|%s'", mode, platform));

                            {
                                DodgyXmlGenerator::Element cl(doc, "ClCompile");

                                doc.SetTextElement("WarningLevel", "Level3");
                                doc.SetTextElement("PrecompiledHeader", "NotUsing");

                                if (mode == "Debug")
                                {
                                    doc.SetTextElement("Optimization", "Disabled");
                                }
                                else
                                {
                                    doc.SetTextElement("Optimization", "MaxSpeed");
                                    doc.SetTextElement("FunctionLevelLinking", "true");
                                    doc.SetTextElement("IntrinsicFunctions", "true");
                                }

                                doc.SetTextElement("PreprocessorDefinitions", preprocessor_defines);
                                doc.SetTextElement("SDLCheck", "true");
                                doc.SetTextElement("CompileAs", "CompileAsCpp");
                                doc.SetTextElement("AdditionalIncludeDirectories", additional_include_dirs);
                                doc.SetTextElement("UseFullPaths", "true");
                                doc.SetTextElement("AdditionalOptions", "/wd4267 /wd4503 %(AdditionalOptions)");
                                doc.SetTextElement("MultiProcessorCompilation", "true");
                                doc.SetTextElement("MinimalRebuild", "false");
                            }

                            {
                                DodgyXmlGenerator::Element cl(doc, "Link");

                                doc.SetTextElement("SubSystem",
                                                   (project_ptr->GetType() == ProjectType::UnitTests) ? "Windows" : "Console");

                                doc.SetTextElement("GenerateDebugInformation", "true");

                                if (mode == "Release")
                                {
                                    doc.SetTextElement("EnableCOMDATFolding", "true");
                                    doc.SetTextElement("OptimizeReferences", "true");
                                }

                                if (project_ptr->GetType() == ProjectType::UnitTests)
                                {
                                    doc.SetTextElement(
                                        "AdditionalLibraryDirectories",
                                        "$(VCInstallDir)UnitTest\\lib;%(AdditionalLibraryDirectories)");
                                }
                            }

                            {
                                // Set all library codec to be explicitly linked in
                                // if any referenced project has requested it

                                bool link_all_objects = false;

                                for (const std::string &ref_project_name : project_ptr->GetSelfPlusReferencesRecursive())
                                {
                                    const Project *ref_project_ptr = GetSolution().GetProject(ref_project_name);

                                    if (ref_project_ptr->IsLinkAllObjectsSet())
                                    {
                                        link_all_objects = true;
                                        break;
                                    }
                                }

                                if (link_all_objects)
                                {
                                    DodgyXmlGenerator::Element cl(doc, "ProjectReference");

                                    doc.SetTextElement("UseLibraryDependencyInputs", "true");
                                }
                            }

                            {
                                // Get all additional library dependencies - recursive through
                                // all referenced projects

                                std::set<std::string> libraries;

                                for (const std::string &ref_project_name : project_ptr->GetSelfPlusReferencesRecursive())
                                {
                                    const Project *ref_project_ptr = GetSolution().GetProject(ref_project_name);

                                    for (const std::string &lib_name : ref_project_ptr->GetAdditionalDependencies())
                                    {
                                        libraries.insert(lib_name);
                                    }
                                }

                                if (!libraries.empty())
                                {
                                    std::string param;
                                    for (const std::string &entry : libraries)
                                    {
                                        if (!param.empty())
                                            param.push_back(';');
                                        param.append(entry);
                                    }

                                    DodgyXmlGenerator::Element lib(doc, "Lib");

                                    doc.SetTextElement("AdditionalDependencies", param);
                                }
                            }
                        }
                     }

                    // Source files

                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &source : project_ptr->GetSources())
                        {
                            DodgyXmlGenerator::Element cl_compile(doc, "ClCompile");
                            doc.SetAttribute("Include", FileUtils::ToWindowsPath(source));
                        }
                    }

                    // Project References (only for applications)

                    if ((project_ptr->GetType() == ProjectType::Application)
                        || (project_ptr->GetType() == ProjectType::UnitTests))
                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        std::set<std::string> refs = project_ptr->GetReferencesRecursive();
                        
                        for (const std::string &ref : refs)
                        {
                            const Project *ref_project_ptr = GetSolution().GetProject(ref);

                            DodgyXmlGenerator::Element project_reference(doc, "ProjectReference");

                            doc.SetAttribute(
                                "Include",
                                Format("%s\\%s\\%s.vcxproj",
                                    path_to_solution_folder,
                                    FileUtils::ToWindowsPath(ref_project_ptr->GetRelativePath()),
                                    ref));

                            doc.SetTextElement("Project", m_project_guids[ref]);
                        }
                    }

                    // Header files

                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &header : project_ptr->GetHeaders())
                        {
                            DodgyXmlGenerator::Element cl_include(doc, "ClInclude");
                            doc.SetAttribute("Include", FileUtils::ToWindowsPath(header));
                        }
                    }

                    // Also put in an entry for the solution file

                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");
                        DodgyXmlGenerator::Element none(doc, "None");
                        doc.SetAttribute("Include", project_ptr->GetFileName());
                    }

                    // Custom build steps

                    for (const Project::CustomBuild &custom_build : project_ptr->GetCustomBuilds())
                    {
                        for (std::string platform : {"Win32", "x64"})
                        {
                            std::string exe = Format("%s%s\\$(Configuration)\\%s.exe",
                                                     path_to_solution_folder,
                                                     (platform == "Win32") ? "" : Format("\\%s", platform),
                                                     custom_build.tool);
                            std::string inputs_joined = boost::algorithm::join(custom_build.inputs, ";");
                            std::string outputs_joined = boost::algorithm::join(custom_build.outputs, ";");

                            DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                            doc.SetAttribute(
                                "Condition",
                                Format("'$(Platform)'=='%s'", platform));

                            DodgyXmlGenerator::Element custom_build_element(doc, "CustomBuild");
                            doc.SetAttribute("Include", inputs_joined);
                            doc.SetTextElement("Command", Format("%s -i %s -o %s %s", exe, inputs_joined, outputs_joined, custom_build.extra_args));
                            doc.SetTextElement("Outputs", outputs_joined);
                            doc.SetTextElement("AdditionalInputs", exe);
                        }
                    }

                    // Last imports

                    {
                        DodgyXmlGenerator::Element import(doc, "Import");

                        doc.SetAttribute(
                            "Project",
                            "$(VCTargetsPath)\\Microsoft.Cpp.targets");
                    }

                    {
                        DodgyXmlGenerator::Element import_group(doc, "ImportGroup");

                        doc.SetAttribute("Label", "ExtensionTargets");
                    }
                }

                // Write the document to the project file

                std::string contents = doc.str();

                boost::replace_all(contents, "\n", "\r\n");

                WriteFile(
                    relative_path + "\\" + name + ".vcxproj",
                    contents);
            }

            void
            VisualStudioGenerator::GenerateProjectFilters(const Project *project_ptr)
            {
                const std::string &name = project_ptr->GetName();
                std::string relative_path = FileUtils::ToWindowsPath(project_ptr->GetRelativePath());

                DodgyXmlGenerator doc;
                {
                    DodgyXmlGenerator::Element tle(doc, "Project");
                    doc.SetAttribute("ToolsVersion", "4.0");
                    doc.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

                    // First, search through all the source and headers and generate
                    // the set of filters

                    std::set<std::string> filter_names;

                    {
                        for (const std::string &source : project_ptr->GetSources())
                        {
                            std::string path = FileUtils::ToWindowsPath(source);
                            while (true)
                            {
                                size_t pos = path.rfind('\\');
                                if (pos == std::string::npos)
                                    break;

                                path.resize(pos);
                                filter_names.insert(path);
                            }
                        }

                        for (const std::string &header : project_ptr->GetHeaders())
                        {
                            std::string path = FileUtils::ToWindowsPath(header);
                            while (true)
                            {
                                size_t pos = path.rfind('\\');
                                if (pos == std::string::npos)
                                    break;

                                path.resize(pos);
                                filter_names.insert(path);
                            }
                        }

                        for (const std::string &resource : project_ptr->GetResources())
                        {
                            std::string path = resource;
                            while (true)
                            {
                                size_t pos = path.rfind('\\');
                                if (pos == std::string::npos)
                                    break;

                                path.resize(pos);
                                filter_names.insert(path);
                            }
                        }
                    }

                    // Create a UUID for each filter and write them out
                    // into an item group

                    if (!filter_names.empty())
                    {
                        // First, we need to generate a name-space UUID.
                        // I've just generated a random Type 1 (time based)
                        // UUID to use as the namespace

                        Uuid namespace_uuid =
                        { 0x1C, 0x2E, 0x9E, 0x30,
                        0x96, 0x89,
                        0x11, 0xE3,
                        0xA5, 0xE2,
                        0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };

                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &filter: filter_names)
                        {
                            DodgyXmlGenerator::Element filter_element(doc, "Filter");

                            doc.SetAttribute("Include", filter);

                            std::string full_path = bbox::Format("%s\\project-%s\\%s",
                                                        FileUtils::ToWindowsPath(project_ptr->GetRelativePath()),
                                                        project_ptr->GetName(),
                                                        filter);

                            std::string filter_guid;
                            {
                                Uuid uuid;
                                uuid.GenerateNamed(namespace_uuid, full_path);

                                filter_guid = uuid.ToString();
                            }

                            doc.SetTextElement("UniqueIdentifier", filter_guid);
                        }
                    }

                    // Finally, write the sources and headers
                    // into two item groups

                    // Sources

                    if (!project_ptr->GetSources().empty())
                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &raw_source : project_ptr->GetSources())
                        {
                            std::string source = FileUtils::ToWindowsPath(raw_source);

                            DodgyXmlGenerator::Element cl_compile(doc, "ClCompile");
                            doc.SetAttribute("Include", source);

                            size_t pos = source.rfind('\\');
                            if (pos != std::string::npos)
                            {
                                doc.SetTextElement("Filter", source.substr(0, pos));
                            }
                        }
                    }

                    // Headers

                    if (!project_ptr->GetHeaders().empty())
                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &raw_header : project_ptr->GetHeaders())
                        {
                            std::string header = FileUtils::ToWindowsPath(raw_header);

                            DodgyXmlGenerator::Element cl_include(doc, "ClInclude");
                            doc.SetAttribute("Include", header);

                            size_t pos = header.rfind('\\');
                            if (pos != std::string::npos)
                            {
                                doc.SetTextElement("Filter", header.substr(0, pos));
                            }
                        }
                    }

                    // Resources

                    if (!project_ptr->GetResources().empty())
                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");

                        for (const std::string &raw_resource : project_ptr->GetResources())
                        {
                            std::string resource = FileUtils::ToWindowsPath(raw_resource);

                            DodgyXmlGenerator::Element custom_build(doc, "CustomBuild");
                            doc.SetAttribute("Include", resource);

                            size_t pos = resource.rfind('\\');
                            if (pos != std::string::npos)
                            {
                                doc.SetTextElement("Filter", resource.substr(0, pos));
                            }
                        }
                    }

                    // Also, add an entry
                    // for the solution file

                    {
                        DodgyXmlGenerator::Element item_group(doc, "ItemGroup");
                        DodgyXmlGenerator::Element none(doc, "None");
                        doc.SetAttribute("Include", FileUtils::ToWindowsPath(project_ptr->GetFileName()));
                    }
                }

                std::string contents = doc.str();

                boost::replace_all(contents, "\n", "\r\n");

                WriteFile(
                    relative_path + "\\" + name + ".vcxproj.filters",
                    contents);
            }

            void VisualStudioGenerator::GenerateTypescriptCsproj(const Project *project_ptr)
            {
                const std::string &name = project_ptr->GetName();
                const std::string &relative_path = project_ptr->GetRelativePath();
                const std::string &guid = m_project_guids[name];

                DodgyXmlGenerator doc;
                {
                    // Top level project elemennt

                    DodgyXmlGenerator::Element tle(doc, "Project");

                    doc.SetAttribute("ToolsVersion", "4.0");
                    doc.SetAttribute("DefaultTargets", "Build");
                    doc.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

                    // Import element

                    {
                        DodgyXmlGenerator::Element import_el(doc, "Import");

                        doc.SetAttribute("Project", "$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\TypeScript\\Microsoft.TypeScript.Default.props");
                        doc.SetAttribute("Condition", "Exists('$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\TypeScript\\Microsoft.TypeScript.Default.props')");
                    }

                    // Property Group

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "PropertyGroup");

                        {
                            DodgyXmlGenerator::Element cfg(doc, "Configuration");
                            doc.SetAttribute("Condition", " '$(Configuration)' == '' ");
                            doc.SetText("Debug");
                        }

                        doc.SetTextElement(
                            "ProjectGuid",
                            guid);
                        doc.SetTextElement(
                            "ProjectTypeGuids",
                            "{349c5851-65df-11da-9384-00065b846f21};{fae04ec0-301f-11d3-bf4b-00c04f79efbc}");
                        doc.SetTextElement(
                            "OutputType",
                            "Library");
                        doc.SetTextElement(
                            "OutputPath",
                            "bin");
                        doc.SetTextElement(
                            "TargetFrameworkVersion",
                            "v4.5.2");
                        doc.SetTextElement(
                            "DebugType",
                            "full");
                        doc.SetTextElement(
                            "DebugSymbols",
                            "true");
                        doc.SetTextElement(
                            "TypeScriptToolsVersion",
                            "1.8");
                        doc.SetTextElement(
                            "UseIISExpress",
                            "true");

                        DodgyXmlGenerator::Element(doc, "IISExpressSSLPort");
                        DodgyXmlGenerator::Element(doc, "IISExpressAnonymousAuthentication");
                        DodgyXmlGenerator::Element(doc, "IISExpressWindowsAuthentication");
                        DodgyXmlGenerator::Element(doc, "IISExpressUseClassicPipelineMode");
                        DodgyXmlGenerator::Element(doc, "UseGlobalApplicationHostFile");
                    }

                    // Item group - service

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "ItemGroup");
                        DodgyXmlGenerator::Element service(doc, "Service");
                        doc.SetAttribute("Include", "{4A0DDDB5-7A95-4FBF-97CC-616D07737A77}");
                    }

                    // Item group - resources

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "ItemGroup");

                        for (const auto &resource : project_ptr->GetResources())
                        {
                            DodgyXmlGenerator::Element content(doc, "Content");
                            doc.SetAttribute("Include", resource);
                        }

                        DodgyXmlGenerator::Element web_config(doc, "Content");
                        doc.SetAttribute("Include", "web.config");
                    }

                    // Item group - type script

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "ItemGroup");

                        // Sources listed in this project

                        for (const auto &source : project_ptr->GetSources())
                        {
                            DodgyXmlGenerator::Element compile(doc, "TypeScriptCompile");
                            doc.SetAttribute("Include", bbox::FileUtils::ToWindowsPath(source));
                        }

                        // Sources listed in dependent projects

                        const std::string path_to_solution = CalcPathToSolution(project_ptr);

                        for (const auto &ref_proj_name : project_ptr->GetReferencesRecursive())
                        {
                            const Project * const ref_proj_ptr = GetSolution().GetProject(ref_proj_name);

                            for (const auto &source : ref_proj_ptr->GetSources())
                            {
                                DodgyXmlGenerator::Element compile(doc, "TypeScriptCompile");
                                doc.SetAttribute(
                                    "Include",
                                    bbox::Format("%s\\%s\\%s",
                                        path_to_solution,
                                        bbox::FileUtils::ToWindowsPath(ref_proj_ptr->GetRelativePath()),
                                        bbox::FileUtils::ToWindowsPath(source)));
                            }
                        }
                    }

                    // Item group - debug/release

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "ItemGroup");

                        for (const auto &mode : { "Debug", "Release" })
                        {
                            DodgyXmlGenerator::Element none(doc, "None");
                            doc.SetAttribute(
                                "Include",
                                bbox::Format("web.%s.config", mode));
                            doc.SetTextElement(
                                "DependentUpon",
                                "web.config");
                        }
                    }

                    // Property groups

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "PropertyGroup");

                        DodgyXmlGenerator::Element version(doc, "VisualStudioVersion");
                        doc.SetAttribute("Condition", "'$(VisualStudioVersion)' == ''");
                        doc.SetText("12.0");
                    }

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "PropertyGroup");
                        doc.SetTextElement("RootNamespace", project_ptr->GetName());
                    }

                    // Imports

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "Import");
                        doc.SetAttribute("Project", "$(MSBuildBinPath)\\Microsoft.CSharp.targets");
                    }

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "Import");
                        doc.SetAttribute("Project", "$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\WebApplications\\Microsoft.WebApplication.targets");
                        doc.SetAttribute("Condition", "Exists('$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\WebApplications\\Microsoft.WebApplication.targets')");
                    }

                    // Project Extensions

                    {
                        DodgyXmlGenerator::Element ext(doc, "ProjectExtensions");
                        DodgyXmlGenerator::Element vs(doc, "VisualStudio");
                        DodgyXmlGenerator::Element flav(doc, "FlavorProperties");
                        doc.SetAttribute("GUID", "{349c5851-65df-11da-9384-00065b846f21}");
                        DodgyXmlGenerator::Element web(doc, "WebProjectProperties");

                        doc.SetTextElement("UseIIS", "True");
                        doc.SetTextElement("AutoAssignPort", "True");
                        doc.SetTextElement("DevelopmentServerPort", bbox::Format("%d", m_iis_port));
                        doc.SetTextElement("DevelopmentServerVPath", "/");
                        doc.SetTextElement("IISUrl", bbox::Format("http://localhost:%d/", m_iis_port));
                        doc.SetTextElement("NTLMAuthentication", "False");
                        doc.SetTextElement("UseCustomServer", "False");
                        doc.SetTextElement("CustomServerUrl", "");
                        doc.SetTextElement("SaveServerSettingsInUserFile", "False");

						// Increment the IIS port for the next project

						m_iis_port += 1;
                    }

                    // Debug/Release properties

                    for (std::string mode : {"Debug", "Release"})
                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "PropertyGroup");
                        doc.SetAttribute(
                            "Condition",
                            bbox::Format("'$(Configuration)' == '%s'", mode));

                        doc.SetTextElement(
                            "TypeScriptRemoveComments",
                            (mode == "Debug") ? "false" : "true");

                        doc.SetTextElement(
                            "TypeScriptSourceMap",
                            "true");

                        doc.SetTextElement(
                            "TypeScriptOutFile",
                            "app.js");

                        doc.SetTextElement(
                            "TypeScriptNoImplicitAny",
                            "True");

                        doc.SetTextElement(
                            "TypeScriptModuleKind",
                            "System");
                    }
                    // Last import

                    {
                        DodgyXmlGenerator::Element prop_grp(doc, "Import");
                        doc.SetAttribute("Project", "$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\TypeScript\\Microsoft.TypeScript.targets");
                        doc.SetAttribute("Condition", "Exists('$(MSBuildExtensionsPath32)\\Microsoft\\VisualStudio\\v$(VisualStudioVersion)\\TypeScript\\Microsoft.TypeScript.targets')");
                    }
                }

                std::string contents = doc.str();

                boost::replace_all(contents, "\n", "\r\n");

                WriteFile(
                    relative_path + "\\" + name + ".csproj",
                    contents);
            }

            void VisualStudioGenerator::GenerateTypescriptWebConfigs(const Project *project_ptr)
            {
                const std::string &relative_path = project_ptr->GetRelativePath();

                // web.config
                
                {
                    DodgyXmlGenerator doc;

                    {
                        DodgyXmlGenerator::Element config(doc, "configuration");
                        DodgyXmlGenerator::Element sys(doc, "system.web");

                        {
                            DodgyXmlGenerator::Element comp(doc, "compilation");
                            doc.SetAttribute("debug", "true");
                            doc.SetAttribute("targetFramework", "4.5");
                        }

                        {
                            DodgyXmlGenerator::Element comp(doc, "httpRuntime");
                            doc.SetAttribute("targetFramework", "4.5");
                        }
                    }

                    std::string contents = doc.str();

                    boost::replace_all(contents, "\n", "\r\n");

                    WriteFile(
                        relative_path + "\\web.config",
                        contents);
                }

                // web.X.config

                for (std::string mode : { "Debug", "Release" })
                {
                    DodgyXmlGenerator doc;

                    {
                        DodgyXmlGenerator::Element config(doc, "configuration");

                        doc.SetAttribute("xmlns:xdt", "http://schemas.microsoft.com/XML-Document-Transform");

                        DodgyXmlGenerator::Element sys(doc, "system.web");

                        if (mode == "Release")
                        {
                            DodgyXmlGenerator::Element comp(doc, "compilation");

                            doc.SetAttribute("xdt:Transform", "RemoveAttributes(debug)");
                        }
                    }

                    std::string contents = doc.str();

                    boost::replace_all(contents, "\n", "\r\n");

                    WriteFile(
                        bbox::Format("%s\\web.%s.config",
                            relative_path,
                            mode),
                        contents);
                }
            }

         } // namespace bbox::make::vs
    } // namesoace bbox::make
} // namespace bbox
