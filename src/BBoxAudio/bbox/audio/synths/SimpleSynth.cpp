/**
 * /file
 *
 * Implementation file for the bbox::audio::synths::SimpleSynth class.
 */

#include <bbox/audio/synths/SimpleSynth.h>

namespace bbox {
namespace audio {
namespace synths {

class SimpleSynth::Note
{
private:
	uint8_t note;
	float phase;
	float omega;
	float amp;
	bool off;

public:
	explicit Note(const midi::MidiEvent &evt, size_t sample_rate)
	{
		note = evt.GetNoteNumber();
		phase = 0.0f;
		omega = 2.0f * 3.1415926535897932384626433832795f * evt.GetNoteFreq() / sample_rate;
		amp = 0.1f * evt.GetVelocityNumber() / 255.0f;
		off = false;
	}

	uint8_t GetNoteNumber() const
	{
		return note;
	}

	void TurnOff()
	{
		off = true;
	}

	bool IsOff() const
	{
		return off && (amp <= 0.0f);
	}

	void SumSteroSamples(float *buffer, size_t num_samples)
	{
		for (size_t i = 0; i < 2 * num_samples; i += 2)
		{
			float value = amp * sinf(phase);

			buffer[i] += value;
			buffer[i + 1] += value;

			phase += omega;
			if (phase > 3.1415926535897932384626433832795f)
				phase -= 3.1415926535897932384626433832795f;

			if (off)
			{
				if (amp <= 0.001f)
					amp = 0.0f;
				else
					amp -= 0.001f;
			}
		}
	}
};

SimpleSynth::SimpleSynth()
	: m_lru()
{
	for (uint8_t i = 0; i < 127; ++i)
	{
		m_by_note[i] = nullptr;
	}
}

SimpleSynth::~SimpleSynth()
{
	while (!m_lru.empty())
	{
		delete m_lru.front();
		m_lru.pop_front();
	}
}

void SimpleSynth::ProcessEvents(const std::vector<midi::MidiEvent> &events, size_t sample_rate)
{
	for (const auto &evt : events)
	{
		switch (evt.EventType())
		{
		case midi::MidiEvent::NOTE_OFF:
			if (m_by_note[evt.GetNoteNumber()] != 0)
				m_by_note[evt.GetNoteNumber()]->TurnOff();
			break;

		case midi::MidiEvent::NOTE_ON:
			if (m_by_note[evt.GetNoteNumber()] == 0)
			{
				Note *note_ptr = new Note(evt, sample_rate);
				m_by_note[evt.GetNoteNumber()] = note_ptr;
				m_lru.push_back(note_ptr);
			}
			break;
		}
	}
}

void SimpleSynth::GenerateStereoSamples(float *buffer, size_t num_samples)
{
	// Silence output

	for (size_t i = 0; i < 2 * num_samples; ++i)
	{
		buffer[i] = 0.0f;
	}

	// Generate samples

	for (auto note_ptr : m_lru)
	{
		note_ptr->SumSteroSamples(buffer, num_samples);
	}

	// Delete any terminated notes

	while (true)
	{
		bool deleted_any = false;

		for (auto it = m_lru.begin();
			it != m_lru.end();
			++it)
		{
			Note *note_ptr = *it;

			if (note_ptr->IsOff())
			{
				m_by_note[note_ptr->GetNoteNumber()] = nullptr;
				m_lru.erase(it);
				delete note_ptr;
				deleted_any = true;
				break;
			}
		}

		if (!deleted_any)
			break;
	}
}

} // namespace bbox::audio::synths
} // namespace bbox::audio
} // namespace bbox
