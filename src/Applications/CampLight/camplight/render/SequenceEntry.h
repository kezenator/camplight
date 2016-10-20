/**
 * @file
 *
 * Header file for the camplight::render::SequenceEntry class.
 */

#ifndef __CAMPLIGHT__RENDER__SEQUENCE_ENTRY_H__
#define __CAMPLIGHT__RENDER__SEQUENCE_ENTRY_H__

#include <bbox/rt/TimeSpan.h>

#include <bbox/enc/FromBinary.h>
#include <bbox/enc/FromTextFormat.h>
#include <bbox/enc/ToBinary.h>
#include <bbox/enc/ToTextFormat.h>

namespace camplight
{
    namespace render
    {

        /**
         * An entry in a sequence of pattern displays.
         */
        class SequenceEntry
        {
        public:
            SequenceEntry() = default;
            SequenceEntry(const SequenceEntry &) = default;
            SequenceEntry &operator =(const SequenceEntry &) = default;
            SequenceEntry(SequenceEntry &&) = default;
            SequenceEntry &operator =(SequenceEntry &&) = default;
            ~SequenceEntry() = default;

            SequenceEntry(const std::string &_top_transition,
                          const std::string &_top_pattern,
                          const std::string &_main_transition,
                          const std::string &_main_pattern,
                          bbox::rt::TimeSpan _duration)
                : top_transition_name(_top_transition)
                , top_pattern_name(_top_pattern)
                , main_transition_name(_main_transition)
                , main_pattern_name(_main_pattern)
                , duration(_duration)
            {
            }

            std::string top_transition_name;
            std::string top_pattern_name;
            std::string main_transition_name;
            std::string main_pattern_name;
            bbox::rt::TimeSpan duration;

            void ToBinary(bbox::enc::ToBinary &m) const;
            void ToTextFormat(bbox::enc::ToTextFormat &m) const;
            void FromBinary(bbox::enc::FromBinary &m);
            void FromTextFormat(bbox::enc::FromTextFormat &m);
        };

    } // namespace camplight::render
} // namespace camplight

#endif // __CAMPLIGHT__RENDER__SEQUENCE_ENTRY_H__
