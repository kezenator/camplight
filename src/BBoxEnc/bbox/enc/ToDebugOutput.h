/**
 * @file
 *
 * Header file for the bbox::enc::ToDebugOutput template function.
 *
 */

#ifndef __BBOX__ENC__TO_DEBUG_OUTPUT_H__
#define __BBOX__ENC__TO_DEBUG_OUTPUT_H__

#include <bbox/enc/ToXml.h>
#include <bbox/DebugOutput.h>

namespace bbox
{
    namespace enc
    {

        /**
         * Converts any marshalable value to a debug string.
         */
        template <typename Type>
        void ToDebugOutput(bbox::DebugOutput &out, const Type &value)
        {
            if (out)
            {
                out.ostream() << ToXml::ConvertToPretty("value", value);
            }
        }

    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__TO_DEBUG_OUTPUT_H__
