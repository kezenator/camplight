/**
* @file
*
* Header file for the linux version of the unit testing library.
*/

#ifndef __UNITTEST__UNIT_TEST_H__
#define __UNITTEST__UNIT_TEST_H__

#include <string>
#include <iostream>
#include <map>
#include <typeinfo>

namespace unittest
{
    inline void Log(const std::string &str)
    {
        std::cout << str << std::endl;
    }
    
    class TestClass
    {
    protected:
        explicit TestClass(const std::type_info &type);
        
    public:

        static int Main(int argc, char *argv[]);

        virtual ~TestClass();
        void RegisterTestMethod(const char *name, void (*func)());

    private:
        const std::type_info &m_type;
        std::map<std::string, void (*)()> m_methods;
        static std::map<std::string, TestClass *> &g_map_instance();
    };

    template <typename DerivedClass>
    class TemplatedTestClass: public TestClass
    {
    protected:
        TemplatedTestClass()
            : TestClass(typeid(DerivedClass))
        {
        }
    };

} // namespace unittest

#define UT_TEST_CLASS(test_class_name) class test_class_name: public ::unittest::TemplatedTestClass<test_class_name>
#define UT_TEST_METHOD(test_method_name) static void test_method_name()

#define UT_REGISTER_CLASS(test_class_name) static test_class_name g_test_ ## test_class_name
#define UT_REGISTER_METHOD(test_method_name) RegisterTestMethod(#test_method_name, &test_method_name);

#include <unittest/Assert.h>

#endif // __UNITTEST__UNIT_TEST_H__
