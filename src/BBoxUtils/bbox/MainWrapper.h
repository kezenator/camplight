/**
 * @file
 *
 * Header file for the BBOX_MAIN_WRAPPER macro.
 */

#ifndef __BBOX__MAIN_WRAPPER_H__
#define __BBOX__MAIN_WRAPPER_H__

#pragma once

#ifdef WIN32

#include <vector>
#include <string>

namespace bbox {
    namespace details {

        void MainWrapperConvert(int argc, wchar_t *argv[], std::vector<std::string> &strings, std::vector<char *> &pointers);

    } // namespace bbox::details
} // namespace bbox

/**
 * Inserts the platform's correct main routine, converts
 * any parameters to the internal UTF-8 format, and calls
 * the specified function, which must have the
 * signature:
 * @code
 * int app_main(int argc, char *argv[])
 * @endcode
 *
 * For example:
 * @code
 * #include <bbox/MainWrapper.h>
 *
 * int my_main(int argc, char *argv[])
 * {
 * }
 *
 * BBOX_MAIN_WRAPPER(my_main);
 * @endcode
 */
#define BBOX_MAIN_WRAPPER(app_main) \
    int wmain(int argc, wchar_t *argv[]) \
    { \
        std::vector<std::string> argv_strings; \
        std::vector<char *> argv_pointers; \
        bbox::details::MainWrapperConvert(argc, argv, argv_strings, argv_pointers); \
        return app_main(argc, &argv_pointers[0]); \
    }

#else // not WIN32

/**
 * Inserts the platform's correct main routine, converts
 * any parameters to the internal UTF-8 format, and calls
 * the specified function, which must have the
 * signature:
 * @code
 * int app_main(int argc, char *argv[])
 * @endcode
 *
 * For example:
 * @code
 * #include <bbox/MainWrapper.h>
 *
 * int my_main(int argc, char *argv[])
 * {
 * }
 *
 * BBOX_MAIN_WRAPPER(my_main);
 * @endcode
 */
#define BBOX_MAIN_WRAPPER(app_main) \
    int main(int argc, char *argv[]) \
    { \
        return app_main(argc, argv); \
    }

#endif // not WIN32

#endif // __BBOX__MAIN_WRAPPER_H__
