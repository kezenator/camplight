/**
 * @file
 *
 * Implementation file for the bbox::http::debug::api::DebugReport class.
 */

#include <bbox/http/debug/api/DebugReport.h>

namespace bbox
{
    namespace http
    {
        namespace debug
        {
            namespace api
            {

                void DebugReport::ToBinary(bbox::enc::ToBinary &m) const
                {
                    m.Write(contents_html);
                }

                void DebugReport::ToTextFormat(bbox::enc::ToTextFormat &m) const
                {
                    m.StartStructure();
                    m.AddNamedValue("contents_html", contents_html);
                    m.CompleteStructure();
                }

                void DebugReport::FromBinary(bbox::enc::FromBinary &m)
                {
                    m.Read(contents_html);
                }

                void DebugReport::FromTextFormat(bbox::enc::FromTextFormat &m)
                {
                    m.StartStructure();
                    m.DecodeNamedValue("contents_html", contents_html);
                    m.CompleteStructure();
                }

            }
        }
    }
}
