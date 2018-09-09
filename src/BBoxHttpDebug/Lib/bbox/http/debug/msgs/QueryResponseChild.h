/**
 * @file
 *
 * Header for the bbox::http::debug::msgs::QueryResponseChild struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_http_debug_msgs.bbidl"
 */

#ifndef __bbox_http_debug_msgs_QueryResponseChild_H__
#define __bbox_http_debug_msgs_QueryResponseChild_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace bbox {
namespace http {
namespace debug {
namespace msgs {

class QueryResponseChild
{
public:
    QueryResponseChild() = default;
    ~QueryResponseChild() = default;
    QueryResponseChild(const QueryResponseChild&) = default;
    QueryResponseChild(QueryResponseChild&&) = default;
    QueryResponseChild &operator =(const QueryResponseChild&) = default;
    QueryResponseChild &operator =(QueryResponseChild&&) = default;

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
    void FromTextFormat(bbox::enc::FromTextFormat &m);

    std::string name;
    std::string path;
    std::string short_description;
};

} // namespace msgs
} // namespace debug
} // namespace http
} // namespace bbox

#endif // __bbox_http_debug_msgs_QueryResponseChild_H__
