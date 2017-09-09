/**
 * @file
 *
 * Implementation file for the bbox::http::debug::api::QueryResultEntry class.
 */

#include <bbox/http/debug/api/QueryResultEntry.h>

namespace bbox
{
    namespace http
    {
        namespace debug
        {
            namespace api
            {

                void QueryResultEntry::ToBinary(bbox::enc::ToBinary &m) const
                {
                    m.Write(path);
                    m.Write(parent);
                    m.Write(children);
                    m.Write(debug_report);
                }

                void QueryResultEntry::ToTextFormat(bbox::enc::ToTextFormat &m) const
                {
                    m.StartStructure();
                    m.AddNamedValue("path", path);
                    m.AddNamedValue("parent", parent);
                    m.AddNamedValue("children", children);
                    m.AddNamedValue("debug_report", debug_report);
                    m.CompleteStructure();
                }

                void QueryResultEntry::FromBinary(bbox::enc::FromBinary &m)
                {
                    m.Read(path);
                    m.Read(parent);
                    m.Read(children);
                    m.Read(debug_report);
                }

                void QueryResultEntry::FromTextFormat(bbox::enc::FromTextFormat &m)
                {
                    m.StartStructure();
                    m.DecodeNamedValue("path", path);
                    m.DecodeNamedValue("parent", parent);
                    m.DecodeNamedValue("children", children);
                    m.DecodeNamedValue("debug_report", debug_report);
                    m.CompleteStructure();
                }

            }
        }
    }
}
