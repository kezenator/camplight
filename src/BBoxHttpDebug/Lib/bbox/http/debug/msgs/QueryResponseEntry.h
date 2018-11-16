/**
 * @file
 *
 * Header for the bbox::http::debug::msgs::QueryResponseEntry struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_http_debug_msgs.bbidl"
 */

#ifndef __bbox_http_debug_msgs_QueryResponseEntry_H__
#define __bbox_http_debug_msgs_QueryResponseEntry_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/MsgTypeLibrary.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

#include <bbox/http/debug/msgs/DebugReportNotification.h>
#include <bbox/http/debug/msgs/QueryResponseChild.h>

namespace bbox {
namespace http {
namespace debug {
namespace msgs {

class QueryResponseEntry
{
public:
    QueryResponseEntry() = default;
    ~QueryResponseEntry() = default;
    QueryResponseEntry(const QueryResponseEntry&) = default;
    QueryResponseEntry(QueryResponseEntry&&) = default;
    QueryResponseEntry &operator =(const QueryResponseEntry&) = default;
    QueryResponseEntry &operator =(QueryResponseEntry&&) = default;

    void ToBinary(bbox::enc::ToBinary &m) const;
    void FromBinary(bbox::enc::FromBinary &m);
    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
    void FromTextFormat(bbox::enc::FromTextFormat &m);

    bool operator ==(const QueryResponseEntry &other) const
    {
        return (path == other.path)
            && (parent_path == other.parent_path)
            && (children == other.children)
            && (report == other.report);
    }

    bool operator !=(const QueryResponseEntry &other) const
    {
        return (path != other.path)
            || (parent_path != other.parent_path)
            || (children != other.children)
            || (report != other.report);
    }

    std::string path;
    std::string parent_path;
    std::vector<QueryResponseChild> children;
    DebugReportNotification report;
};

} // namespace msgs
} // namespace debug
} // namespace http
} // namespace bbox

#endif // __bbox_http_debug_msgs_QueryResponseEntry_H__
