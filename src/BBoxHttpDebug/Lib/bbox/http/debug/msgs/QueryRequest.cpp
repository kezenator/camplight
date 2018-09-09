/**
 * @file
 *
 * Source for the bbox::http::debug::msgs::QueryRequest struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_http_debug_msgs.bbidl"
 */

#include <bbox/http/debug/msgs/QueryRequest.h>

namespace bbox {
namespace http {
namespace debug {
namespace msgs {

void QueryRequest::ToBinary(bbox::enc::ToBinary &m) const
{
    m.Write(request_id);
    m.Write(query);
}

void QueryRequest::FromBinary(bbox::enc::FromBinary &m)
{
    m.Read(request_id);
    m.Read(query);
}

void QueryRequest::ToTextFormat(bbox::enc::ToTextFormat &m) const
{
    m.StartStructure();
    m.AddNamedValue("request_id", request_id);
    m.AddNamedValue("query", query);
    m.CompleteStructure();
}

void QueryRequest::FromTextFormat(bbox::enc::FromTextFormat &m)
{
    m.StartStructure();
    m.DecodeNamedValue("request_id", request_id);
    m.DecodeNamedValue("query", query);
    m.CompleteStructure();
}

} // namespace msgs
} // namespace debug
} // namespace http
} // namespace bbox

