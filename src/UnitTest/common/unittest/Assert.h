/**
 * @file
 *
 * Header file for the linux version of the unit testing unittest::Assert class.
 */

#ifndef __UNITTEST__ASSERT_H__
#define __UNITTEST__ASSERT_H__

#include <bbox/Assert.h>
#include <bbox/Format.h>
#include <bbox/ToString.h>

#define UT_LINE_INFO() ::unittest::LineInfo(__FILE__, __LINE__)

namespace unittest
{

    struct LineInfo
    {
        const char * const file;
        const int line;
        
        LineInfo() = default;
        LineInfo(const LineInfo &) = default;
        LineInfo &operator =(const LineInfo &) = default;
        
        LineInfo(const char *_file, int _line)
            : file(_file)
            , line(_line)
        {
        }
    };

    struct Assert
    {
        static void IsTrue(bool condition, const std::string &description, const LineInfo &line_info)
        {
            if (!condition)
            {
                Log("Error: Assert::IsTrue failed\n");
                Log(bbox::Format("Description: %s\n", description));
                Log(bbox::Format("Location: %s(%d)\n", line_info.file, line_info.line));
                BBOX_ASSERT(false);
            }
        }

        static void IsFalse(bool condition, const std::string &description, const LineInfo &line_info)
        {
            if (condition)
            {
                Log("Error: Assert::IsFalse failed");
                Log(bbox::Format("Description: %s", description));
                Log(bbox::Format("Location: %s(%d)", line_info.file, line_info.line));
                BBOX_ASSERT(false);
            }
        }

        template <typename T, typename U>
        static void AreEqual(const T &expected, const U &value, const std::string &description, const LineInfo &line_info)
        {
            if (!(expected == value))
            {
                Log("Error: Assert::AreEqual failed");
                Log(bbox::Format("Description: %s", description));
                Log(bbox::Format("Location: %s(%d)", line_info.file, line_info.line));
                Log(bbox::Format("Expected: %s", bbox::ToString(expected)));
                Log(bbox::Format("Value: %s", bbox::ToString(value)));
                BBOX_ASSERT(false);
            }
        }

        template <typename T, typename U>
        static void AreNotEqual(const T &expected, const U &value, const std::string &description, const LineInfo &line_info)
        {
            if (!(expected != value))
            {
                Log("Error: Assert::AreNotEqual failed");
                Log(bbox::Format("Description: %s", description));
                Log(bbox::Format("Location: %s(%d)", line_info.file, line_info.line));
                Log(bbox::Format("Expected: %s", bbox::ToString(expected)));
                Log(bbox::Format("Value: %s", bbox::ToString(value)));
                BBOX_ASSERT(false);
            }
        }
    };

} // namespace unittest

#endif // __UNITTEST__ASSERT_H__
