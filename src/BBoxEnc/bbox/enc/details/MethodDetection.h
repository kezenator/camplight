/**
 * @file
 *
 * Header file for the bbox::enc::details::MethodDetection template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__METHOD_DETECTION_H__
#define __BBOX__ENC__DETAILS__METHOD_DETECTION_H__

#include <type_traits>
#include <utility>
#include <bbox/ToString.h>

namespace bbox
{
    namespace enc
    {
        // Forward declarations
        class FromBinary;
        class ToBinary;
        class ToTextFormat;
        class FromTextFormat;

        namespace details
        {

            /**
             * A template that determines if a given type
             * has a set of methods as required for the various
             * encoding classes.
             */
            template <typename _ValueType>
            class MethodDetection
            {
            public:
                /**
                 * The type that the template was instantiated with.
                 */
                using ValueType = _ValueType;

            private:
                template <typename T>
                static constexpr auto check_from_binary(T *)
                    -> typename std::enable_if<
                                    std::is_same<
                                        decltype(std::declval<T>().FromBinary(*static_cast<enc::FromBinary*>(nullptr))),
                                        void>::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_from_binary(...);

                template <typename T>
                static constexpr auto check_to_binary(T*)
                    -> typename std::enable_if<
                                    std::is_same<
                                        decltype(std::declval<const T>().ToBinary(*static_cast<enc::ToBinary*>(nullptr))),
                                        void>::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_to_binary(...);

                template <typename T>
                static constexpr auto check_to_text_format(T*)
                    -> typename std::enable_if<
                                    std::is_same<
                                        decltype(std::declval<const T>().ToTextFormat(*static_cast<enc::ToTextFormat*>(nullptr))),
                                        void>::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_to_text_format(...);

                template <typename T>
                static constexpr auto check_from_text_format(T*)
                    -> typename std::enable_if<
                                    std::is_same<
                                        decltype(std::declval<T>().FromTextFormat(*static_cast<enc::FromTextFormat*>(nullptr))),
                                        void>::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_from_text_format(...);

                template <typename T>
                static constexpr auto check_to_string(T*)
                    -> typename std::enable_if<
                                    std::is_assignable<
                                        std::string,
                                        decltype(std::declval<const T>().ToString())
                                    >::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_to_string(...);

                template <typename T>
                static constexpr auto check_from_string(T*)
                    -> typename std::enable_if<
                                    std::is_same<
                                        bool,
                                        decltype(T::FromString(std::declval<const std::string>(), *static_cast<T*>(nullptr)))
                                    >::value,
                                    std::true_type
                                >::type;

                template <typename T>
                static constexpr std::false_type check_from_string(...);

            public:
                /**
                 * Indicates if the ValueType has a compatible FromBinary method.
                 */
                static constexpr bool HasFromBinary = decltype(check_from_binary<ValueType>(static_cast<ValueType *>(nullptr)))::value;

                /**
                 * Indicates if the ValueType has a compatible ToBinary method.
                 */
                static constexpr bool HasToBinary = decltype(check_to_binary<ValueType>(static_cast<ValueType *>(nullptr)))::value;

                /**
                 * Indicates if the ValueType has a compatible ToTextFormat method.
                 */
                static constexpr bool HasToTextFormat = decltype(check_to_text_format<ValueType>(static_cast<ValueType *>(nullptr)))::value;

                /**
                 * Indicates if the ValueType has a compatible FromTextFormat method.
                 */
                static constexpr bool HasFromTextFormat = decltype(check_from_text_format<ValueType>(static_cast<ValueType *>(nullptr)))::value;

                /**
                 * Indicates if the ValueType has a compatible ToString method.
                 */
                static constexpr bool HasToString = decltype(check_to_string<ValueType>(static_cast<ValueType *>(nullptr)))::value;

                /**
                 * Indicates if the ValueType has a compatible FromString method.
                 */
                static constexpr bool HasFromString = decltype(check_from_string<ValueType>(static_cast<ValueType *>(nullptr)))::value;
            };

        } // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__METHOD_DETECTION_H__
