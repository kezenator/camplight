/**
 * /file
 *
 * Header file for the bbox::audio::midi::MidiEvent class.
 */

#ifndef __BBOX__AUDIO__MIDI__MIDI_EVENT_H__
#define __BBOX__AUDIO__MIDI__MIDI_EVENT_H__

#include <cstdint>
#include <cmath>

namespace bbox {
namespace audio {
namespace midi {

/**
 * A MIDI event.
 */
class MidiEvent
{
public:

	enum EVENT_TYPE
	{
		NOTE_OFF = 0x80,
		NOTE_ON = 0x90,
	};

	MidiEvent()
		: m_event(0)
	{
	}

	explicit MidiEvent(uint32_t val)
		: m_event(val)
	{
	}

	MidiEvent(const MidiEvent &) = default;
	MidiEvent &operator =(const MidiEvent &) = default;
	~MidiEvent() = default;

	EVENT_TYPE EventType() const
	{
		return EVENT_TYPE(m_event & 0xF0);
	}

	uint8_t GetNoteNumber() const
	{
		return (m_event >> 8) & 0x7F;
	}

	float GetNoteFreq() const
	{
		// A4 = 440Hz = Midi note 69
		// each note is 12th root of 2 factor change

		return 440.0f * powf(2.0f, (GetNoteNumber() - 69) / 12.0f);
	}

	uint8_t GetVelocityNumber() const
	{
		return (m_event >> 16) & 0x7f;
	}

private:
	uint32_t m_event;
};

} // namespace bbox::audio::midi
} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__MIDI__MIDI_EVENT_H__