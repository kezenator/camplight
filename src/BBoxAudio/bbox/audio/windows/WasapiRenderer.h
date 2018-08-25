/**
 * /file
 *
 * Header file for the bbox::audio::windows::WasapiRenderer class.
 */

#ifndef __BBOX__AUDIO__WINDOWS__WASAPI_RENDERER_H__
#define __BBOX__AUDIO__WINDOWS__WASAPI_RENDERER_H__

#include <bbox/Error.h>

#include <memory.h>

namespace bbox {
namespace audio {
namespace windows {

/**
 * A Wasapi render stream.
 */
class WasapiRenderer
{
public:
	WasapiRenderer();
	~WasapiRenderer();

	void *GetEventHandle();

	bbox::Error OpenAndStart();
	void StopAndClose();

	size_t GetSampleRate();
	bbox::Error OpenRenderBuffer(float *&buffer_ptr, size_t &buffer_frames);
	bbox::Error CompleteRenderBuffer();

private:
	struct Pimpl;
	std::unique_ptr<Pimpl> m_pimpl;
};

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox

#endif // __BBOX__AUDIO__WINDOWS__WASAPI_RENDERER_H__