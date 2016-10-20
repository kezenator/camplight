/**
* @file
*
* Implementation file for the bbox::make::Generator class.
*/

#include <bbox/make/Generator.h>
#include <bbox/make/Solution.h>
#include <bbox/FileUtils.h>

namespace bbox {
    namespace make {

        Generator::Generator(const Solution &solution)
            : m_solution(solution)
            , m_any_files_changed(false)
        {
        }

        Generator::~Generator()
        {
        }

        void
        Generator::WriteFile(const std::string &relative_path,
                             const std::string &contents)
        {
            // Create the full file path from the solution
            // base dir and the relative path

            std::string file_name =
                m_solution.GetBaseFolder()
                + FileUtils::FOLDER_SEPERATOR
                + relative_path;

            // First attempt to read the current contents and
            // work out if it already has this contents

            bool need_write = false;

            {
                std::string cur_contents;
                Error err = FileUtils::ReadTextFile(file_name, cur_contents);

                if (err)
                    need_write = true;
                else if (cur_contents != contents)
                    need_write = true;
            }

            // Try and attempt to write the new file

            if (need_write)
            {
                m_any_files_changed = true;

                std::cout
                    << "   Updating file "
                    << relative_path
                    << "..."
                    << std::endl;

                FileUtils::WriteTextFile(file_name, contents);
            }
        }

    } // namesoace bbox::make
} // namespace bbox
