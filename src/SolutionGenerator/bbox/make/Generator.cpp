/**
* @file
*
* Implementation file for the bbox::make::Generator class.
*/

#include <bbox/make/Generator.h>
#include <bbox/make/Solution.h>
#include <bbox/Assert.h>
#include <bbox/FileUtils.h>
#include <bbox/TextCoding.h>

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
        Generator::WriteFile_UnixNewline(const std::string &relative_path,
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
                std::vector<uint8_t> bytes;
                std::string cur_contents;

                Error err = FileUtils::ReadBinaryFile(file_name, bytes, 100 * 1024 * 1024);

                if (!err)
                {
                    err = TextCoding::ExternalBytes_to_UTF8(bytes, cur_contents);
                }

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

                std::vector<uint8_t> bytes;

                bbox::Error err = TextCoding::UTF8_to_ExternalBytes(contents, bytes);
                BBOX_ASSERT(!err);

                FileUtils::WriteBinaryFileOrThrow(file_name, bytes);
            }
        }

        void
        Generator::WriteFile_DosNewline(const std::string &relative_path,
                                        const std::string &contents)
        {
            WriteFile_UnixNewline(
                relative_path,
                TextCoding::Newlines_UNIX_to_DOS(contents));
        }

    } // namesoace bbox::make
} // namespace bbox
