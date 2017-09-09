/**
 * @file
 *
 * Header file for the bbox::http::debug::api::ChildEntry class.
 */

#ifndef __BBOX__HTTP__DEBUG__API__CHILD_ENTRY_H__
#define __BBOX__HTTP__DEBUG__API__CHILD_ENTRY_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace bbox
{
    namespace http
    {
        namespace debug
        {
            namespace api
            {

                /**
                 * A report generated from the debug output.
                 */
                struct ChildEntry
                {
                    std::string name;
                    std::string path;
                    std::string short_description;

                    ChildEntry() = default;
                    ChildEntry(const ChildEntry &) = default;
                    ChildEntry(ChildEntry &&) = default;
                    ~ChildEntry() = default;
                    ChildEntry &operator =(const ChildEntry &) = default;
                    ChildEntry &operator =(ChildEntry &&) = default;

                    ChildEntry(const std::string &_name, const std::string &_path, const std::string &_short_description)
                        : name(_name)
                        , path(_path)
                        , short_description(_short_description)
                    {
                    }

                    void ToBinary(bbox::enc::ToBinary &m) const;
                    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
                    void FromBinary(bbox::enc::FromBinary &m);
                    void FromTextFormat(bbox::enc::FromTextFormat &m);
                };
            }
        }
    }
}

#endif // v
