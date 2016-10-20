/**
 * @file
 *
 * Header file for the windows version of the unit testing library.
 */

#include <unittest/UnitTest.h>
#include <bbox/TypeInfo.h>

namespace unittest
{

    std::map<std::string, TestClass *> &TestClass::g_map_instance()
    {
        static std::map<std::string, TestClass *> g_test_classes;
        
        return g_test_classes;
    }

    TestClass::TestClass(const std::type_info &type)
        : m_type(type)
    {
        g_map_instance()[bbox::TypeInfo(m_type).pretty_name()] = this;
    }

    TestClass::~TestClass()
    {
        g_map_instance().erase(bbox::TypeInfo(m_type).pretty_name());
    }

    void TestClass::RegisterTestMethod(const char *name, void (*func)())
    {
        m_methods[name] = func;
    }
    
    int TestClass::Main(int argc, char *argv[])
    {
        for (const auto &class_entry : g_map_instance())
        {
            const TestClass *class_ptr = class_entry .second;

            for (const auto &method_entry : class_ptr->m_methods)
            {
                std::cout
                    << "--- "
                    << bbox::TypeInfo(class_ptr->m_type).pretty_name()
                    << "::"
                    << method_entry.first
                    << " ---"
                    << std::endl;

                method_entry.second();
            }
        }

        return 0;
    }

} // namespace unittest

int main(int argc, char *argv[])
{
    return unittest::TestClass::Main(argc, argv);
}