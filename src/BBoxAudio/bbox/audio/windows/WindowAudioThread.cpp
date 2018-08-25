/**
 * /file
 *
 * Implementation file for the bbox::audio::windows::WindowsAudioThread class.
 */

#include <bbox/audio/windows/WindowAudioThread.h>
#include <bbox/Assert.h>

#include <Objbase.h>

namespace bbox {
namespace audio {
namespace windows {

WindowsAudioThread::WindowsAudioThread(const std::string &name, rt::Service &parent)
	: details::AudioThread(name, parent)
	, m_thread()
	, m_stopping(false)
	, m_event(INVALID_HANDLE_VALUE)
	, m_renderer()
	, m_midi_in()
	, m_simple_synth()
{
	m_event = ::CreateEvent(nullptr, false, false, nullptr);
	BBOX_ASSERT(m_event != INVALID_HANDLE_VALUE);

	m_thread = std::thread(std::bind(&WindowsAudioThread::ThreadFunc, this));
}

WindowsAudioThread::~WindowsAudioThread()
{
	::CloseHandle(m_event);
}

void WindowsAudioThread::HandleStopping()
{
	m_stopping.store(true);
}

void WindowsAudioThread::PrintState(bbox::DebugOutput &out) const
{
	out.Format("TODO\n");
}

void WindowsAudioThread::Join()
{
	m_thread.join();
	RequestStopAllChildren();
	NotifyStopped();
}

void WindowsAudioThread::ThreadFunc()
{
	::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	bbox::Error error;

	error = m_midi_in.OpenAndStart();
	// TODO
	BBOX_ASSERT(!error);

	error = m_renderer.OpenAndStart();
	// TODO
	BBOX_ASSERT(!error);

	float phase = 0.0f;

	while (m_stopping.load() == false)
	{
		::WaitForSingleObjectEx(m_renderer.GetEventHandle(), INFINITE, false);

		float *buffer_ptr;
		size_t buffer_frames;
		error = m_renderer.OpenRenderBuffer(buffer_ptr, buffer_frames);

		m_simple_synth.ProcessEvents(m_midi_in.GetEvents(), m_renderer.GetSampleRate());
		m_simple_synth.GenerateStereoSamples(buffer_ptr, buffer_frames);

		m_renderer.CompleteRenderBuffer();
	}

	m_renderer.StopAndClose();
	m_midi_in.StopAndClose();

	GetProactor().ThreadSafePost(std::bind(&WindowsAudioThread::Join, this));
}

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox
