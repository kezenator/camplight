/**
 * @file
 *
 * Header file for the bbox::enc::api::describe parameter direction types.
 */

#ifndef __BBOX__ENC__API__DESCRIBE__DIRECTIONS_H__
#define __BBOX__ENC__API__DESCRIBE__DIRECTIONS_H__

namespace bbox
{
    namespace enc
    {
        namespace api
        {
            namespace describe
            {
                /**
                 * The type for input parameters.
                 */
                struct Input
                {
                    static constexpr bool IsInput = true;
                    static constexpr bool IsOutput = false;

                    static constexpr bool Include(bool for_input)
                    {
                        return for_input;
                    }
                };

                /**
                * The type for output parameters.
                */
                struct Output
                {
                    static constexpr bool IsInput = false;
                    static constexpr bool IsOutput = true;

                    static constexpr bool Include(bool for_input)
                    {
                        return !for_input;
                    }
                };

                /**
                * The type for in/out (reference) parameters.
                */
                struct InOut
                {
                    static constexpr bool IsInput = true;
                    static constexpr bool IsOutput = true;

                    static constexpr bool Include(bool /*for_input*/)
                    {
                        return true;
                    }
                };

            } // namespace bbox::enc::api::describe
        } // namespace bbox::enc::api
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__API__DESCRIBE__DIRECTIONS_H__
