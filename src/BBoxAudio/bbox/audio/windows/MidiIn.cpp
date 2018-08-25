/**
 * /file
 *
 * Implementation file for the bbox::audio::windows::MidiIn class.
 */

#include <bbox/audio/windows/MidiIn.h>

#include <atomic>
#include <mutex>

#include <Windows.h>

namespace bbox {
namespace audio {
namespace windows {

struct MidiIn::Pimpl
{
	HMIDIIN m_handle;
	std::atomic<bool> m_closed;
	std::mutex m_mutex;
	std::vector<midi::MidiEvent> m_events;
	float m_freq;
	float m_amp;

	Pimpl()
	{
		m_freq = 1000.0f;
		m_amp = 0.0f;

		MMRESULT result;

		result = midiInOpen(
			&m_handle,
			0,
			reinterpret_cast<DWORD_PTR>(&MidiInProc),
			reinterpret_cast<DWORD_PTR>(this),
			CALLBACK_FUNCTION);

		if (result != MMSYSERR_NOERROR)
		{
			m_handle = nullptr;
			return;
		}

		result = midiInStart(m_handle);

		if (result != MMSYSERR_NOERROR)
		{
			midiInClose(m_handle);
			m_handle = nullptr;
			return;
		}

		m_closed.store(false);
	}

	~Pimpl()
	{
		if (m_handle != nullptr)
		{
			midiInStop(m_handle);
			midiInClose(m_handle);
			m_handle = nullptr;

			while (m_closed.load() == false)
				;
		}
	}
	
	std::vector<midi::MidiEvent> GetEvents()
	{
		std::vector<midi::MidiEvent> result;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			std::swap(result, m_events);
		}

		return result;
	}

	static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		Pimpl *pimpl = reinterpret_cast<Pimpl *>(dwInstance);

		switch (wMsg)
		{
		case MIM_CLOSE:
			pimpl->m_closed.store(true);
			break;

		case MIM_DATA:
			{
			    std::lock_guard<std::mutex> lock(pimpl->m_mutex);
				pimpl->m_events.emplace_back(dwParam1);
			}
			break;
		}
	}

};

MidiIn::MidiIn()
{
}

MidiIn::~MidiIn()
{
}

bbox::Error MidiIn::OpenAndStart()
{
	m_pimpl = std::make_unique<Pimpl>();

	return bbox::Error();
}

void MidiIn::StopAndClose()
{
	m_pimpl.reset();
}

std::vector<midi::MidiEvent> MidiIn::GetEvents()
{
	if (m_pimpl)
		return m_pimpl->GetEvents();

	return std::vector<midi::MidiEvent>();
}

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox
