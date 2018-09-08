/**
 * @file
 *
 * Implementation file for the BBoxEncCompiler application.
 */

#include <bbox/MainWrapper.h>
#include <bbox/FileUtils.h>
#include <iostream>

#include <bbox/enc/compiler/Compiler.h>

int bbox_enc_compiler_main(int argc, char *argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage:" << std::endl;
			std::cerr << "   BBoxEncCompiler idl_src_path" << std::endl;
			std::cerr << std::endl;
			std::cerr << "   idl_src_path - folder containing the IDL source files" << std::endl;
			return 1;
		}

		std::cerr << "Reading Sources..." << std::endl;

		auto src_file_list = bbox::FileUtils::ListFolderOrThrow(
			std::string(argv[1]) + "/*.bbidl",
			bbox::FileInfo::RegularFile);

		std::vector<bbox::enc::compiler::File::ptr> src_file_contents;

		for (const bbox::FileInfo &entry : src_file_list)
		{
			src_file_contents.emplace_back(std::make_shared<bbox::enc::compiler::File>(
				std::string(entry.GetName()),
				bbox::FileUtils::ReadTextFileOrThrow(entry.GetFullPath())));
		}

		std::cerr << "Compiling..." << std::endl;

		bbox::enc::compiler::Compiler compiler;

		std::map<std::string, std::string> outputs;
		std::vector<std::string> error_strings;

		bool result = compiler.CompileSources(
			std::move(src_file_contents),
			outputs,
			error_strings);

		for (const auto &str : error_strings)
			std::cerr << str << std::endl;

		if (!result)
			return 1;

		// Now, generate the outputs

		std::cerr << "Generating outputs..." << std::endl;

		for (const auto &entry : outputs)
		{
			const std::string &path = entry.first;
			const std::string &contents = entry.second;

			std::string existing_contents;

			bbox::Error read_err = bbox::FileUtils::ReadTextFile(path, existing_contents);

			if (read_err)
			{
				std::cerr << "Creating: " << path << std::endl;
			}
			else if (contents == existing_contents)
			{
				continue;
			}
			else
			{
				std::cerr << "Updating: " << path << std::endl;
			}

			bbox::FileUtils::WriteTextFileOrThrow(path, contents);
		}

		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}
	catch (...)
	{
		std::cerr << "Unkown exception" << std::endl;

		return 1;
	}
}

BBOX_MAIN_WRAPPER(bbox_enc_compiler_main)
