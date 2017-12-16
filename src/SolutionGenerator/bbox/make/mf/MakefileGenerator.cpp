/**
* @file
*
* Implementation file for the bbox::make::mf::MakefileGenerator class.
*/

#include <bbox/make/mf/MakefileGenerator.h>
#include <bbox/make/Solution.h>
#include <bbox/make/Project.h>

#include <bbox/Assert.h>
#include <bbox/Format.h>
#include <bbox/Uuid.h>
#include <bbox/Exception.h>

#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

namespace bbox {
    namespace make {
        namespace mf {

            struct MakefileGenerator::project_sorter
            {
                size_t Depth(const Project *p) const
                {
                    size_t result = 1;

                    for (const std::string &ref_name : p->GetReferences())
                    {
                        const Project *ref = p->GetSolution().GetProject(ref_name);
                        size_t d = 1 + Depth(ref);
                        if (d > result)
                            result = d;
                    }

                    return result;
                }

                bool operator()(const Project *p1, const Project *p2) const
                {
                    size_t d1 = Depth(p1);
                    size_t d2 = Depth(p2);

                    if (d1 < d2)
                        return true;
                    else if (d1 > d2)
                        return false;
                    else
                        return p1->GetName() < p2->GetName();
                }
            };

            MakefileGenerator::MakefileGenerator(const Solution &solution)
                : Generator(solution)
            {
            }

            MakefileGenerator::~MakefileGenerator()
            {
            }

            void
            MakefileGenerator::UpdateFiles()
            {
                // Sort the projects, putting the low level ones
                // first then the big applications

                std::vector<const Project *> projects = GetSolution().GetProjects();
                std::set<const Project *, project_sorter> sorted_projects(
                    projects.begin(),
                    projects.end());

                std::stringstream stream;

                for (const Project *proj : sorted_projects)
                {
                    stream << "# Project " << proj->GetName()
                        << " (type " << proj->GetType().ToString()
                        << ")" << std::endl;
                    stream << std::endl;

                    stream << "MODULES += " << proj->GetName() << std::endl;
                    stream << std::endl;
                                        
                    switch (proj->GetType().GetValue())
                    {
                    case ProjectType::Application:
                        {
                            PrintSources(stream, proj);
                            PrintIncludes(stream, proj);
                            PrintLibs(stream, proj);
                            PrintCustomBuildSteps(stream, proj);

                            stream << "APPS += " << proj->GetName() << std::endl;
                            stream << std::endl;
                        }
                        break;
                    case ProjectType::TypeScriptApplication:
                    case ProjectType::TypeScriptLibrary:
                        {
                            throw Exception(bbox::Format("Project %s: TypeScriptApplication/Library projects not supported via Makefile",
                                proj->GetName()));
                        }
                        break;
                    case ProjectType::StaticLibrary:
                        {
                            PrintSources(stream, proj);
                            PrintIncludes(stream, proj);
                            PrintCustomBuildSteps(stream, proj);
                    }
                        break;
                    case ProjectType::UnitTests:
                        {
                            PrintSources(stream, proj);
                            PrintIncludes(stream, proj);
                            PrintLibs(stream, proj);
                            PrintCustomBuildSteps(stream, proj);

                            stream << "TESTS += " << proj->GetName() << std::endl;
                            stream << std::endl;
                    }
                        break;
                    }
                }

                WriteFile_UnixNewline("modules.make", stream.str());
            }

            void MakefileGenerator::PrintSources(std::ostream &stream, const Project *proj)
            {
                stream << "SRCS_DIRECT_" << proj->GetName() << " := $(sort \\" << std::endl;

                for (const std::string &source : proj->GetSources())
                {
                    stream << "        "
                        << UnixPath(Format("%s/%s", proj->GetRelativePath(), source))
                        << " \\"
                        << std::endl;
                }
                stream << "    )" << std::endl;
                stream << "    # End SRCS_DIRECT_" << proj->GetName() << std::endl;
                stream << std::endl;

                stream << "SRCS_RECURSIVE_" << proj->GetName() << " := $(sort \\" << std::endl;
                stream << "        $(SRCS_DIRECT_" << proj->GetName() << ") \\" << std::endl;

                for (const std::string &ref_name : proj->GetReferences())
                {
                    stream << "        $(SRCS_RECURSIVE_" << ref_name << ") \\" << std::endl;
                }

                stream << "    )" << std::endl;
                stream << "    # End SRCS_RECURSIVE_" << proj->GetName() << std::endl;
                stream << std::endl;
            }

            void MakefileGenerator::PrintIncludes(std::ostream &stream, const Project *proj)
            {
                stream << "INCS_DIRECT_" << proj->GetName() << " := $(sort \\" << std::endl;

                for (const std::string &source : proj->GetIncludes())
                {
                    stream << "        "
                        << UnixPath(Format("%s/%s", proj->GetRelativePath(), source))
                        << " \\"
                        << std::endl;
                }
                stream << "    )" << std::endl;
                stream << "    # End INCS_DIRECT_" << proj->GetName() << std::endl;
                stream << std::endl;

                stream << "INCS_RECURSIVE_" << proj->GetName() << " := $(sort \\" << std::endl;
                stream << "        $(INCS_DIRECT_" << proj->GetName() << ") \\" << std::endl;

                for (const std::string &ref_name : proj->GetReferences())
                {
                    stream << "        $(INCS_RECURSIVE_" << ref_name << ") \\" << std::endl;
                }

                stream << "    )" << std::endl;
                stream << "    # End INCS_RECURSIVE_" << proj->GetName() << std::endl;
                stream << std::endl;
            }

            void MakefileGenerator::PrintLibs(std::ostream &stream, const Project *proj)
            {
                // Get the set of projects we include libraries from,
                // sorted so that libraries with no dependencies are at the front

                std::set<const Project *, project_sorter> ref_project_ptrs;

                for (const std::string &proj_name : proj->GetSelfPlusReferencesRecursive())
                {
                    ref_project_ptrs.insert(GetSolution().GetProject(proj_name));
                }

                // Create the list of libraries from each project, with libraries
                // from less dependant projects at the back

                std::deque<std::string> lib_names;
                std::set<std::string> already_added;

                for (const Project *ref_ptr : ref_project_ptrs)
                {
                    for (const std::string lib_name : ref_ptr->GetAdditionalDependencies())
                    {
                        if (already_added.count(lib_name) == 0)
                        {
                            lib_names.push_front(lib_name);
                        }
                    }
                }

                // Now print it all

                stream << "LIBS_RECURSIVE_" << proj->GetName() << " := \\" << std::endl;

                for (const std::string &lib : lib_names)
                {
                    stream << "        " << lib << " \\" << std::endl;
                }
                stream << "    # End LIBS_RECURSIVE_" << proj->GetName() << std::endl;
                stream << std::endl;
            }

            void MakefileGenerator::PrintCustomBuildSteps(std::ostream &stream, const Project *proj)
            {
                for (const Project::CustomBuild &custom_build : proj->GetCustomBuilds())
                {
                    std::string prefix = UnixPath(proj->GetRelativePath());
                    prefix.push_back('/');

                    stream << PrefixJoinToUnix(prefix, custom_build.outputs, " ")
                        << ": "
                        << "build/" << boost::algorithm::to_lower_copy(custom_build.tool)
                        << " "
                        << PrefixJoinToUnix(prefix, custom_build.inputs, " ")
                        << std::endl;

                    stream << "\t"
                        << "cd " << UnixPath(proj->GetRelativePath())
                        << " && " << UnixPath(proj->CalculateReverseRelativePath()) << "/build/" << boost::algorithm::to_lower_copy(custom_build.tool)
                        << " -i \""
                        << PrefixJoinToUnix("", custom_build.inputs, ";")
                        << "\" -o \""
                        << PrefixJoinToUnix("", custom_build.outputs, ";")
                        << "\" "
                        << custom_build.extra_args
                        << std::endl;

                    stream << std::endl;
                }
            }

            std::string MakefileGenerator::UnixPath(std::string path)
            {
                for (size_t index = 0; index < path.size(); ++index)
                {
                    if (path[index] == '\\')
                        path[index] = '/';
                }
                return path;
            }

            std::string MakefileGenerator::PrefixJoinToUnix(const std::string prefix, const std::set<std::string> &files, const std::string &sep)
            {
                std::stringstream stream;
                bool first = true;

                for (const std::string &file : files)
                {
                    if (first)
                        first = false;
                    else
                        stream << sep;
                    stream << prefix << UnixPath(file);
                }

                return stream.str();
            }

         } // namespace bbox::make::mf
    } // namesoace bbox::make
} // namespace bbox
