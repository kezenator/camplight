/**
 * @file
 *
 * Header file for the bbox::enc::ToDebugString template function.
 *
 */

#ifndef __BBOX__ENC__TO_DEBUG_STRING_H__
#define __BBOX__ENC__TO_DEBUG_STRING_H__

#include <bbox/enc/ToJson.h>

namespace bbox
{
    namespace enc
    {

        /**
         * Converts any marshalable value to a debug string.
         */
        template <typename Type>
        std::string ToDebugString(const Type &value)
        {
            ToJson to_json(ToTextFormat::HUMAN_DEBUGGING);
            to_json.SetValue(value);
            return to_json.ToString();
        }


    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__TO_DEBUG_STRING_H__
