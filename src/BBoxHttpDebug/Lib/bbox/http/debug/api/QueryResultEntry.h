/**
 * @file
 *
 * Header file for the bbox::http::debug::api::QueryResultEntry class.
 */

#ifndef __BBOX__HTTP__DEBUG__API__QUERY_RESULT_ENTRY_H__
#define __BBOX__HTTP__DEBUG__API__QUERY_RESULT_ENTRY_H__

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

#include <bbox/http/debug/api/ChildEntry.h>
#include <bbox/http/debug/api/DebugReport.h>

namespace bbox
{
    namespace http
    {
        namespace debug
        {
            namespace api
            {

                /**
                 * An entry returned from a debug query.
                 */
                struct QueryResultEntry
                {
                    std::string path;
                    std::string parent;
                    std::vector<ChildEntry> children;
                    DebugReport debug_report;

                    void ToBinary(bbox::enc::ToBinary &m) const;
                    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
                    void FromBinary(bbox::enc::FromBinary &m);
                    void FromTextFormat(bbox::enc::FromTextFormat &m);
                };
            }
        }
    }
}

#endif // __BBOX__HTTP__DEBUG__API__QUERY_RESULT_ENTRY_H__
