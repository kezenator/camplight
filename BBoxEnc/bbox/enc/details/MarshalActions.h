/**
 * @file
 *
 * Header file for the bbox::enc::details::MarshalActions template classes.
 *
 */

#ifndef __BBOX__ENC__DETAILS__MARSHAL_ACTION_H__
#define __BBOX__ENC__DETAILS__MARSHAL_ACTION_H__

#include <bbox/enc/details/MarshalStrategy.h>

namespace bbox
{
    namespace enc
    {
        namespace details
        {

            template <typename Strategy, typename Type>
            struct ToBinaryAction;

            template <typename Strategy, typename Type>
            struct ToTextFormatAction;

            template <typename Strategy, typename Type>
            struct FromBinaryAction;

            template <typename Strategy, typename Type>
            struct FromTextFormatAction;

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__MARSHAL_ACTION_H__
