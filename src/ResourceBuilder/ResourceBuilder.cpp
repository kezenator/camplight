/**
 * @file
 *
 * Main file for a solution that takes a number of input
 * files and generates an output .cpp and .h file that
 * contain copies of the contents of the input files
 * as constant data.
 *
 */

#include <iostream>
#include <set>
#include <vector>
#include <sstream>
#include <map>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <bbox/Exception.h>
#include <bbox/FileUtils.h>
#include <bbox/Format.h>
#include <bbox/MainWrapper.h>

#include <bbox/crypto/HashStream.h>

void UpdateFile(const std::string &name, const std::string &contents)
{
    bool needs_write = false;

#ifdef WIN32

	// When building on Windows, Visual Studio knows about all of the dependencies
	// correctly - so this tool only gets run if something really has changed.
	// If we don't update the time-stamp on the files, the Visual Studio keeps
	// running this build step because the output files are STILL older than
	// the changed input file.
	//
	// As such - we always write the files to update their time stamp.

	needs_write = true;

#else

	// When building using make, I don't have the dependencies fully working
	// yet. So this step is run on every make.
	// We don't want to update source/header files that haven't changed
	// as it prevents make re-building these files on every make.

    try
    {
        std::string current_contents = bbox::FileUtils::ReadTextFileOrThrow(name);

        if (current_contents != contents)
            needs_write = true;
    }
    catch (...)
    {
        // Could not read the file - might be new -
        // in any case try and write it

        needs_write = true;
    }

#endif

    if (needs_write)
    {
        std::cout << "Updating " << name << "..." << std::endl;

        bbox::FileUtils::WriteTextFileOrThrow(name, contents);
    }
}

std::string NamespaceOpen(std::ostream &stream, const std::vector<std::string> &namespace_path)
{
    std::string indent;

    if (!namespace_path.empty())
    {
        std::string name = namespace_path[0];
        stream << "namespace " << name << " {" << std::endl;

        for (size_t i = 1; i < namespace_path.size(); ++i)
        {
            indent.append("    ");

            stream << indent << "namespace " << namespace_path[i] << " {" << std::endl;
        }

        indent.append("    ");

        stream << std::endl;
    }

    return indent;
}

void NamespaceClose(std::ostream &stream, const std::vector<std::string> &namespace_path)
{
    if (!namespace_path.empty())
    {
        stream << std::endl;

        for (size_t i = 0; i < namespace_path.size(); ++i)
        {
            size_t num_names = namespace_path.size() - i;

            std::string indent(4 * (num_names - 1), ' ');

            std::string name;
            for (size_t j = 0; j < num_names; j++)
            {
                if (j != 0)
                    name.append("::");
                name.append(namespace_path[j]);
            }

            stream << indent << "} // namespace " << name << std::endl;
        }
    }
}

std::string FixFileName(const std::string &file_name)
{
    std::string result(file_name);

    for (size_t i = 0; i < result.size(); ++i)
    {
        if (result[i] == '\\')
            result[i] = '/';
    }

    return result;
}

int resource_builder_main(int argc, char *argv[])
{
    try
    {
        // We expect exactly 8 extra arguments - they must be
        // -i "inputs;files"
        // -o "output.cpp;output.h"
        // -n "name::space"
        // -r "remove\\path"

        bool args_valid = false;
        std::set<std::string> inputs;
        std::string output_cpp;
        std::string output_h;
        std::string output_path;
        std::vector<std::string> namespace_path;
        std::string path_remove;

        if (argc == 9)
        {
            if ((strcmp(argv[1], "-i") == 0)
                && (strcmp(argv[3], "-o") == 0)
                && (strcmp(argv[5], "-n") == 0)
                && (strcmp(argv[7], "-r") == 0))
            {
                std::string input_str(argv[2]);
                boost::algorithm::split(inputs, input_str, boost::algorithm::is_any_of(";"));
                if (!inputs.empty())
                {
                    std::string output_str(argv[4]);
                    std::vector<std::string> outputs;
                    boost::algorithm::split(outputs, output_str, boost::algorithm::is_any_of(";"));

                    if ((outputs.size() == 3)
                        && boost::algorithm::ends_with(outputs[0], ".cpp")
                        && boost::algorithm::ends_with(outputs[1], ".h")
                        && boost::algorithm::ends_with(outputs[2], ".path"))
                    {
                        output_cpp = outputs[0];
                        output_h = outputs[1];
                        output_path = outputs[2];

                        // Finally, split the namespace by double-colons
                        std::string ns(argv[6]);
                        if (!ns.empty())
                        {
                            size_t pos = 0;
                            while (pos < ns.size())
                            {
                                size_t index = ns.find("::", pos);
                                if (index == std::string::npos)
                                {
                                    // Insert from pos to end
                                    if (pos < ns.size() - 1)
                                        namespace_path.push_back(ns.substr(pos));
                                    break;
                                }
                                else
                                {
                                    // Insert from pos to index
                                    // and continue the search
                                    if (index > pos)
                                        namespace_path.push_back(ns.substr(pos, index - pos));
                                    pos = index + 2;
                                }
                            }
                        }

                        path_remove = argv[8];
                        args_valid = true;
                    }
                }
            }
        }

        if (!args_valid)
            throw bbox::Exception("Invalid arguments");

        // Setup the MIME type table

        std::map<std::string, std::string> extension_lookup = {
            // Text formats
            { ".txt",       "text/plain" },
            { ".htm",       "text/html" },
            { ".html",      "text/html" },
            { ".css",       "text/css" },
            { ".js",        "application/javascript" },
            // Image formats
            { ".png",       "image/png" },
            { ".jpg",       "image/jpeg" },
            { ".jpeg",      "image/jpeg" },
            { ".gif",       "image/gif" },
        };

        // Generate the source

        std::string source;

        {
            std::stringstream stream;

            stream << "/**" << std::endl;
            stream << " * @file" << std::endl;
            stream << " *" << std::endl;
            stream << " * AUTOMATICALLY GENERATED source file defining the resource file data contents." << std::endl;
            stream << " */" << std::endl;
            stream << std::endl;
            stream << "#include <cstdint>" << std::endl;
            stream << "#include <bbox/http/ResourceFileSet.h>" << std::endl;
            stream << "#include <" << output_h << ">" << std::endl;
            stream << std::endl;
            stream << "#ifdef _DEBUG" << std::endl;
            stream << "#include <" << output_path << ">" << std::endl;
            stream << "#endif" << std::endl;
            stream << std::endl;
            stream << "namespace {" << std::endl;

            std::vector<size_t> file_lengths;
            std::vector<std::string> file_etags;

            size_t count = 0;
            for (const std::string &input : inputs)
            {
                count++;

                std::string contents = bbox::FileUtils::ReadTextFileOrThrow(input);

                file_lengths.push_back(contents.size());

                {
                    bbox::crypto::HashStream hash_stream(bbox::crypto::HashStream::SHA_256);
                    hash_stream.AddBytes(contents.c_str(), contents.size());

                    file_etags.push_back(hash_stream.CompleteHash().ToBase64String());
                }

                stream << "    // File #" << count << " - " << FixFileName(input) << std::endl;
                stream << "    const uint8_t file_contents_" << count << "[" << contents.size() << "] = {" << std::endl;
                for (size_t i = 0; i < contents.size(); ++i)
                {
                    if ((i & 0xF) == 0)
                    {
                        if (i != 0)
                            stream << "," << std::endl;
                        stream << "        ";
                    }
                    else
                        stream << ", ";

                    stream << bbox::Format("0x%02X", contents[i] & 0xFF);
                }

                if (!contents.empty())
                    stream << std::endl;

                stream << "    };" << std::endl;
            }

            stream << "} // annonymous namespace" << std::endl;
            stream << std::endl;

            std::string indent = NamespaceOpen(stream, namespace_path);

            stream << indent << "::bbox::http::ResourceFileSet g_resource_files({" << std::endl;

            std::string prefix = path_remove;
            prefix.push_back('\\');

            count = 0;
            for (const std::string &input : inputs)
            {
                count++;

                std::string mime_type = "application/octet-stream";

                size_t dot_pos = input.rfind('.');
                if (dot_pos != std::string::npos)
                {
                    std::string extension = input.substr(dot_pos);
                    auto it = extension_lookup.find(extension);
                    if (it != extension_lookup.end())
                        mime_type = it->second;
                }

                if (FixFileName(input.substr(0, prefix.size())) != FixFileName(prefix))
                    throw bbox::Exception(bbox::Format("Filename \"%s\" does not begin with expected prefix \"%s\"", input, prefix));

                stream << indent << "    // File #" << count << " - " << FixFileName(input) << std::endl;
                stream << indent << "    {" << std::endl;
                stream << indent << "        \"" << FixFileName(input.substr(prefix.size())) << "\", // Filename" << std::endl;
                stream << indent << "        file_contents_" << count << ", // Contents" << std::endl;
                stream << indent << "        " << file_lengths[count-1] << ", // Length" << std::endl;
                stream << indent << "        \"identity\", // Content Encoding" << std::endl;
                stream << indent << "        \"" << mime_type << "\", // Mime-type" << std::endl;
                stream << indent << "        \"\\\"" << file_etags[count - 1] << "\\\"\", // Strong ETag" << std::endl;
				stream << "#ifdef _DEBUG" << std::endl;
				stream << indent << "        " << "RESOURCE_BASE_PATH \"/\" \"" << bbox::FileUtils::ToUnixPath(input) << "\", // Original file name (for debugging)" << std::endl;
				stream << "#endif" << std::endl;
				stream << indent << "    }," << std::endl;
            }

            stream << indent << "});" << std::endl;

            NamespaceClose(stream, namespace_path);

            source = stream.str();
        }

        // Generate the header

        std::string header;

        {
            std::stringstream stream;

            stream << "/**" << std::endl;
            stream << " * @file" << std::endl;
            stream << " *" << std::endl;
            stream << " * AUTOMATICALLY GENERATED header file declaring the resource file data structure." << std::endl;
            stream << " */" << std::endl;
            stream << std::endl;
            stream << "#include <bbox/http/ResourceFileSet.h>" << std::endl;
            stream << std::endl;

            std::string indent = NamespaceOpen(stream, namespace_path);

            stream << indent << "extern ::bbox::http::ResourceFileSet g_resource_files;" << std::endl;

            NamespaceClose(stream, namespace_path);

            header = stream.str();
        }

        // Generate the path file

        std::string path_file;
       
        {
            std::stringstream stream;

            stream << "/**" << std::endl;
            stream << " * @file" << std::endl;
            stream << " *" << std::endl;
            stream << " * AUTOMATICALLY GENERATED file listing the local path of the source files on this machine." << std::endl;
            stream << " * DO NOT CHECK THIS INTO SOURCE CONTROL!" << std::endl;
            stream << " */" << std::endl;
            stream << std::endl;
            stream << "#define RESOURCE_BASE_PATH \"" << bbox::FileUtils::ToUnixPath(bbox::FileUtils::GetCurrentWorkingDir()) << "\"" << std::endl;

            path_file = stream.str();
        }

        // Output the files

        UpdateFile(output_cpp, source);
        UpdateFile(output_h, header);
        UpdateFile(output_path, path_file);

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }
}

BBOX_MAIN_WRAPPER(resource_builder_main)