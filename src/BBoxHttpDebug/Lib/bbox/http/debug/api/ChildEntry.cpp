/**
 * @file
 *
 * Implementation file for the bbox::http::debug::api::ChildEntry class.
 */

#include <bbox/http/debug/api/ChildEntry.h>

namespace bbox
{
    namespace http
    {
        namespace debug
        {
            namespace api
            {

                void ChildEntry::ToBinary(bbox::enc::ToBinary &m) const
                {
                    m.Write(name);
                    m.Write(path);
                    m.Write(short_description);
                }

                void ChildEntry::ToTextFormat(bbox::enc::ToTextFormat &m) const
                {
                    m.StartStructure();
                    m.AddNamedValue("name", name);
                    m.AddNamedValue("path", path);
                    m.AddNamedValue("short_description", short_description);
                    m.CompleteStructure();
                }

                void ChildEntry::FromBinary(bbox::enc::FromBinary &m)
                {
                    m.Read(name);
                    m.Read(path);
                    m.Read(short_description);
                }

                void ChildEntry::FromTextFormat(bbox::enc::FromTextFormat &m)
                {
                    m.StartStructure();
                    m.DecodeNamedValue("name", name);
                    m.DecodeNamedValue("path", path);
                    m.DecodeNamedValue("short_description", short_description);
                    m.CompleteStructure();
                }

            }
        }
    }
}
