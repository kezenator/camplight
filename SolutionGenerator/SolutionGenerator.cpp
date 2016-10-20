/**
 * @file
 *
 * Main file for the solution generator application that
 * generates Visual Studio solution files, project files,
 * and also GNU make files from a common solution name.
 *
 */

#include <iostream>

#include <bbox/Exception.h>
#include <bbox/FileUtils.h>
#include <bbox/MainWrapper.h>

#include <bbox/make/Solution.h>
#include <bbox/make/vs/VisualStudioGenerator.h>
#include <bbox/make/mf/MakefileGenerator.h>

int solution_generator_main(int argc, char *argv[])
{
    try
    {
        std::string current_dir = bbox::FileUtils::GetCurrentWorkingDir();

        bool files_changed = false;

        {
            bbox::make::Solution solution(current_dir, "Win32");

            solution.ParseMakeFiles();

            bbox::make::vs::VisualStudioGenerator vs_generator(solution);

            vs_generator.UpdateFiles();

            files_changed |= vs_generator.AnyFilesChanged();
        }

        {
            bbox::make::Solution solution(current_dir, "Linux");

            solution.ParseMakeFiles();

            bbox::make::mf::MakefileGenerator mf_generator(solution);

            mf_generator.UpdateFiles();

            files_changed |= mf_generator.AnyFilesChanged();
        }

        if (files_changed)
        {
            std::cout << "Project files have been updated - please make again"
                << std::endl;

            return 1;
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }
}

BBOX_MAIN_WRAPPER(solution_generator_main)
