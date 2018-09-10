/**
 * @file
 *
 * Header file for the bbox::enc::details::MarshalStrategy template class.
 *
 */

#ifndef __BBOX__ENC__DETAILS__MARSHAL_STRATEGY_H__
#define __BBOX__ENC__DETAILS__MARSHAL_STRATEGY_H__

#include <bbox/ToString.h>
#include <bbox/FromString.h>

#include <bbox/enc/MsgPtr.h>

#include <utility>
#include <set>
#include <list>
#include <vector>
#include <map>

namespace bbox
{
    namespace enc
    {
        // Forward declarations
        class FromBinary;
        class ToBinary;
        class ToTextFormat;

        namespace details
        {

            /**
             * A set of strategies for marshaling.
             */
            struct MarshalStrategy
            {
                struct NotSupported {};

                struct AsStdString {};
                struct AsBool {};
                struct AsSignedInt {};
                struct AsUnsignedInt {};
                struct AsBinaryVector {};
                struct AsStdPair {};
                struct AsStdVector {};
                struct AsStdSet {};
                struct AsStdMap {};
				struct AsMsgPtr {};

                struct ViaCustomMethods {};
                struct ViaBinaryCustomMethodsAndToFromString {};
            };

            /**
             * A template that chooses the strategy.
             */
            template <typename Type>
            struct ChooseStrategy
            {
                using Strategy = typename std::conditional<
                    MethodDetection<Type>::HasFromBinary
                        && MethodDetection<Type>::HasToBinary
                        && MethodDetection<Type>::HasToTextFormat
                        && MethodDetection<Type>::HasFromTextFormat,
                    MarshalStrategy::ViaCustomMethods,
                    typename std::conditional<
                        MethodDetection<Type>::HasFromBinary
                            && MethodDetection<Type>::HasToBinary
                            && MethodDetection<Type>::HasToString
                            && MethodDetection<Type>::HasFromString
                            && (!MethodDetection<Type>::HasToTextFormat
                                || !MethodDetection<Type>::HasFromTextFormat),
                        MarshalStrategy::ViaBinaryCustomMethodsAndToFromString,
                        MarshalStrategy::NotSupported
                    >::type
                >::type;
            };

            template <>
            struct ChooseStrategy<std::string>
            {
                using Strategy = MarshalStrategy::AsStdString;
            };

            template <>
            struct ChooseStrategy<bool>
            {
                using Strategy = MarshalStrategy::AsBool;
            };

            template <>
            struct ChooseStrategy<int8_t>
            {
                using Strategy = MarshalStrategy::AsSignedInt;
            };

            template <>
            struct ChooseStrategy<int16_t>
            {
                using Strategy = MarshalStrategy::AsSignedInt;
            };

            template <>
            struct ChooseStrategy<int32_t>
            {
                using Strategy = MarshalStrategy::AsSignedInt;
            };

            template <>
            struct ChooseStrategy<int64_t>
            {
                using Strategy = MarshalStrategy::AsSignedInt;
            };
                                                                              
            template <>
            struct ChooseStrategy<uint8_t>
            {
                using Strategy = MarshalStrategy::AsUnsignedInt;
            };

            template <>
            struct ChooseStrategy<uint16_t>
            {
                using Strategy = MarshalStrategy::AsUnsignedInt;
            };

            template <>
            struct ChooseStrategy<uint32_t>
            {
                using Strategy = MarshalStrategy::AsUnsignedInt;
            };

            template <>
            struct ChooseStrategy<uint64_t>
            {
                using Strategy = MarshalStrategy::AsUnsignedInt;
            };

            template <typename First, typename Second>
            struct ChooseStrategy<std::pair<First, Second>>
            {
                using Strategy = MarshalStrategy::AsStdPair;
            };

            template <typename Value, typename Allocator>
            struct ChooseStrategy<std::vector<Value, Allocator>>
            {
                using Strategy = MarshalStrategy::AsStdVector;
            };

            template <typename Value, typename Comparator, typename Allocator>
            struct ChooseStrategy<std::set<Value, Comparator, Allocator>>
            {
                using Strategy = MarshalStrategy::AsStdSet;
            };

            template <typename Allocator>
            struct ChooseStrategy<std::vector<uint8_t, Allocator>>
            {
                using Strategy = MarshalStrategy::AsBinaryVector;
            };

            template <typename Key, typename Value, typename Compare, typename Allocator>
            struct ChooseStrategy<std::map<Key, Value, Compare, Allocator>>
            {
                using Strategy = MarshalStrategy::AsStdMap;
            };

			template <>
			struct ChooseStrategy<MsgAnyPtr>
			{
				using Strategy = MarshalStrategy::AsMsgPtr;
			};

			template <typename Type>
			struct ChooseStrategy<MsgPtr<Type>>
			{
				using Strategy = MarshalStrategy::AsMsgPtr;
			};

		} // namespace bbox:enc::details
    } // namespace bbox::enc
} // namespace bbox

#endif // __BBOX__ENC__DETAILS__MARSHAL_STRATEGY_H__
