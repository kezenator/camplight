/**
 * @file
 *
 * Implementation file for the camplight::render::SequenceEntry class.
 */

#include <camplight/render/SequenceEntry.h>

namespace camplight
{
    namespace render
    {


        void SequenceEntry::ToBinary(bbox::enc::ToBinary &m) const
        {
            m.Write(top_transition_name);
            m.Write(top_pattern_name);
            m.Write(main_transition_name);
            m.Write(main_pattern_name);
            m.Write(uint64_t(duration.ToMilliseconds()));
        }

        void SequenceEntry::ToTextFormat(bbox::enc::ToTextFormat &m) const
        {
            m.StartStructure();

            m.AddNamedValue("top-transition", top_transition_name);
            m.AddNamedValue("top-pattern", top_pattern_name);
            m.AddNamedValue("main-transition", main_transition_name);
            m.AddNamedValue("main-pattern", main_pattern_name);

            const uint64_t duration_ms = duration.ToMilliseconds();
            m.AddNamedValue("duration-ms", duration_ms);

            m.CompleteStructure();
        }

        void SequenceEntry::FromBinary(bbox::enc::FromBinary &m)
        {
            m.Read(top_transition_name);
            m.Read(top_pattern_name);
            m.Read(main_transition_name);
            m.Read(main_pattern_name);

            uint64_t duration_ms;
            m.Read(duration_ms);
            duration = bbox::rt::TimeSpan::Milliseconds(duration_ms);
        }

        void SequenceEntry::FromTextFormat(bbox::enc::FromTextFormat &m)
        {
            m.StartStructure();

            m.DecodeNamedValue("top-transition", top_transition_name);
            m.DecodeNamedValue("top-pattern", top_pattern_name);
            m.DecodeNamedValue("main-transition", main_transition_name);
            m.DecodeNamedValue("main-pattern", main_pattern_name);

            uint64_t duration_ms;
            m.DecodeNamedValue("duration-ms", duration_ms);
            duration = bbox::rt::TimeSpan::Milliseconds(duration_ms);

            m.CompleteStructure();
        }

    } // namespace camplight::render
} // namespace camplight
