/**
 * @file
 *
 * Implementation file for the BBOX_MAIN_WRAPPER macro.
 */

#include <bbox/MainWrapper.h>
#include <bbox/TextCoding.h>

#ifdef WIN32

namespace bbox {
    namespace details {

        void MainWrapperConvert(int argc, wchar_t *argv[], std::vector<std::string> &strings, std::vector<char *> &pointers)
        {
            strings.resize(argc);
            pointers.resize(argc);

            for (int i = 0; i < argc; i++)
            {
                strings[i] = TextCoding::Win32_UTF16_to_UTF8(argv[i]);
                pointers[i] = &strings[i][0];
            }
        }

    } // namespace bbox::details
} // namespace bbox

#else // not WIN32

    // Nothing required

#endif // not WIN32
