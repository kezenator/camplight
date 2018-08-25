/**
 * /file
 *
 * Header file for the bbox::audio::details::AudioThread class.
 */

#ifndef __BBOX__AUDIO__DETAILS__AUDIO_THREAD_H__
#define __BBOX__AUDIO__DETAILS__AUDIO_THREAD_H__

#include <bbox/rt/Resource.h>

namespace bbox {
namespace audio {
namespace details {

/**
 * Base class for an audio thread.
 */
class AudioThread : public rt::Resource
{
public:
	AudioThread(const std::string &name, rt::Service &parent);
	~AudioThread();
};

} // namespace bbox::audio::details
} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__DETAILS__AUDIO_THREAD_H__