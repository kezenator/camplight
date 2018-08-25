/**
 * /file
 *
 * Header file for the bbox::audio::AudioService class.
 */

#ifndef __BBOX__AUDIO__AUDIO_SERVICE_H__
#define __BBOX__AUDIO__AUDIO_SERVICE_H__

#include <bbox/rt/Service.h>

namespace bbox {
namespace audio {

/**
 * Creates an audio service for rending content.
 */
class AudioService : public rt::Service
{
public:
	AudioService(const std::string &name, rt::Proactor &parent);
	AudioService(const std::string &name, rt::Service &parent);
	~AudioService();

private:
	void HandleStarting() override;
	void HandleStopping() override;
	void PrintState(bbox::DebugOutput &out) const override;

	struct Pimpl;
	std::unique_ptr<Pimpl> m_pimpl;
};

} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__AUDIO_SERVICE_H__