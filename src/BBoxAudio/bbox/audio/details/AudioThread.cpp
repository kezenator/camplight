/**
 * /file
 *
 * Implementation file for the bbox::audio::details::AudioThread class.
 */

#include <bbox/audio/details/AudioThread.h>

namespace bbox {
namespace audio {
namespace details {

AudioThread::AudioThread(const std::string &name, rt::Service &parent)
	: rt::Resource(name, parent)
{
}

AudioThread::~AudioThread()
{
}

} // namespace bbox::audio::details
} // namespace bbox::audio
} // namespace bbox
