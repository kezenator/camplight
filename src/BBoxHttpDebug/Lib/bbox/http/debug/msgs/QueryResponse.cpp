/**
 * @file
 *
 * Source for the bbox::http::debug::msgs::QueryResponse struct.
 *
 * THIS FILE IS AUTOMATICALLY GENERATED BY THE IDL COMPILER FROM
 * IDL SOURCE FILE "bbox_http_debug_msgs.bbidl"
 */

#include <bbox/http/debug/msgs/QueryResponse.h>

namespace bbox {
namespace http {
namespace debug {
namespace msgs {

void QueryResponse::ToBinary(bbox::enc::ToBinary &m) const
{
    m.Write(request_id);
    m.Write(entries);
}

void QueryResponse::FromBinary(bbox::enc::FromBinary &m)
{
    m.Read(request_id);
    m.Read(entries);
}

void QueryResponse::ToTextFormat(bbox::enc::ToTextFormat &m) const
{
    m.StartStructure();
    m.AddNamedValue("request_id", request_id);
    m.AddNamedValue("entries", entries);
    m.CompleteStructure();
}

void QueryResponse::FromTextFormat(bbox::enc::FromTextFormat &m)
{
    m.StartStructure();
    m.DecodeNamedValue("request_id", request_id);
    m.DecodeNamedValue("entries", entries);
    m.CompleteStructure();
}

} // namespace msgs
} // namespace debug
} // namespace http
} // namespace bbox

