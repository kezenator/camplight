/**
 * /file
 *
 * Header file for the bbox::audio::synths::SimpleSynth class.
 */

#ifndef __BBOX__AUDIO__SYNTHS__SIMPLE_SYNTH_H__
#define __BBOX__AUDIO__SYNTHS__SIMPLE_SYNTH_H__

#include <bbox/audio/midi/MidiEvent.h>

#include <list>
#include <vector>

namespace bbox {
namespace audio {
namespace synths {

/**
 * A very simple sythesizer.
 */
class SimpleSynth
{
public:
	SimpleSynth();
	~SimpleSynth();

	void ProcessEvents(const std::vector<midi::MidiEvent> &events, size_t sample_rate);
	void GenerateStereoSamples(float *buffer, size_t num_samples);

private:
	class Note;
	Note *m_by_note[127];
	std::list<Note *> m_lru;
};

} // namespace bbox::audio::synths
} // namespace bbox::audio
} // namespace bbox

#endif // #define __BBOX__AUDIO__SYNTHS__SIMPLE_SYNTH_H__
