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

		bbox::enc::compiler::Compiler compiler;

		std::vector<std::string> error_strings;

		bool result = compiler.CompileSources(
			std::move(src_file_contents),
			error_strings);

		for (const auto &str : error_strings)
			std::cerr << str << std::endl;

		if (!result)
			return 1;

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
