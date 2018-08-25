/**
 * /file
 *
 * Header file for the bbox::audio::windows::WindowsAudioThread class.
 */

#ifndef __BBOX__AUDIO__WINDOWS__WINDOWS_AUDIO_THREAD_H__
#define __BBOX__AUDIO__WINDOWS__WINDOWS_AUDIO_THREAD_H__

#include <bbox/audio/details/AudioThread.h>
#include <bbox/audio/windows/WasapiRenderer.h>
#include <bbox/audio/windows/MidiIn.h>
#include <bbox/audio/synths/SimpleSynth.h>

#include <thread>

#include <Windows.h>

namespace bbox {
namespace audio {
namespace windows {

/**
 * Creates an audio service for rending content.
 */
class WindowsAudioThread : public details::AudioThread
{
public:
	WindowsAudioThread(const std::string &name, rt::Service &parent);
	~WindowsAudioThread();

private:
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	void ThreadFunc();
	void Join();

	std::thread m_thread;
	std::atomic<bool> m_stopping;
	HANDLE m_event;
	WasapiRenderer m_renderer;
	MidiIn m_midi_in;
	synths::SimpleSynth m_simple_synth;
};

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__WINDOWS__WINDOWS_AUDIO_THREAD_H__