/**
* @file
*
* Header file for the windows version of the unit testing library.
*/

#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

#include <CppUnitTest.h>
#include <string>

namespace unittest
{
    inline void Log(const std::string &str)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(str.c_str());
    };

} // namespace unittest

#define UT_TEST_CLASS(test_class_name) TEST_CLASS(test_class_name)
#define UT_TEST_METHOD(test_method_name) TEST_METHOD(test_method_name)

#define UT_REGISTER_CLASS(test_class_name) static test_class_name *g_ ## test_class_name = nullptr;
#define UT_REGISTER_METHOD(test_method_name)

#include <unittest/Assert.h>

#endif // __UNIT_TEST_H__
