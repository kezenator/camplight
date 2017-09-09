/**
 * @file
 *
 * Header file for the bbox::http::debug::api::DebugReport class.
 */

#ifndef __BBOX__HTTP__DEBUG__API__DEBUG_REPORT_H__
#define __BBOX__HTTP__DEBUG__API__DEBUG_REPORT_H__

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
                struct DebugReport
                {
                    std::string contents_html;

                    void ToBinary(bbox::enc::ToBinary &m) const;
                    void ToTextFormat(bbox::enc::ToTextFormat &m) const;
                    void FromBinary(bbox::enc::FromBinary &m);
                    void FromTextFormat(bbox::enc::FromTextFormat &m);
                };
            }
        }
    }
}

#endif // __BBOX__HTTP__DEBUG__API__DEBUG_REPORT_H__
