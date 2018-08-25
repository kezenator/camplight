/**
 * /file
 *
 * Header file for the bbox::audio::windows::MidiIn class.
 */

#ifndef __BBOX__AUDIO__WINDOWS__MIDI_IN_H__
#define __BBOX__AUDIO__WINDOWS__MIDI_IN_H__

#include <bbox/Error.h>

#include <bbox/audio/midi/MidiEvent.h>

#include <memory>
#include <vector>

namespace bbox {
namespace audio {
namespace windows {

/**
 * A MIDI input port.
 */
class MidiIn
{
public:
	MidiIn();
	~MidiIn();

	bbox::Error OpenAndStart();
	void StopAndClose();

	std::vector<midi::MidiEvent> GetEvents();

private:
	struct Pimpl;
	std::unique_ptr<Pimpl> m_pimpl;
};

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__WINDOWS__MIDI_IN_H__