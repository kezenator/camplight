/**
 * /file
 *
 * Implementation file for the bbox::audio::AudioService class.
 */

#include <bbox/audio/AudioService.h>
#include <bbox/audio/windows/WindowAudioThread.h>

namespace bbox {
namespace audio {

struct AudioService::Pimpl
{
	AudioService &m_parent;
	windows::WindowsAudioThread m_thread;

	Pimpl(AudioService &parent)
		: m_parent(parent)
		, m_thread("thread", parent)
	{
	}
};

AudioService::AudioService(const std::string &name, rt::Proactor &parent)
	: rt::Service(name, parent)
	, m_pimpl(std::make_unique<Pimpl>(*this))
{
}

AudioService::AudioService(const std::string &name, rt::Service &parent)
	: rt::Service(name, parent)
	, m_pimpl(std::make_unique<Pimpl>(*this))
{
}

AudioService::~AudioService()
{
}

void AudioService::HandleStarting()
{
	NotifyStarted();
}

void AudioService::HandleStopping()
{
	RequestStopAllChildren();
	NotifyStopped();
}

void AudioService::PrintState(bbox::DebugOutput &out) const
{
	out.Format("TODO\n");
}

} // namespace bbox::audio
} // namespace bbox
