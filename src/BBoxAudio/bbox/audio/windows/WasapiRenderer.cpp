/**
 * /file
 *
 * Implementation file for the bbox::audio::windows::WasapiRenderer class.
 */

#include <bbox/audio/windows/WasapiRenderer.h>
#include <bbox/Assert.h>

#include <Windows.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

namespace bbox {
namespace audio {
namespace windows {

struct WasapiRenderer::Pimpl
{
	HANDLE m_event;
	IMMDeviceEnumerator *m_pEnumerator;
	IMMDevice *m_pDevice;
	IAudioClient *m_pAudioClient;
	IAudioRenderClient *m_pRenderClient;
	WAVEFORMATEX *m_pwfx;
	REFERENCE_TIME m_hnsRequestedDuration = 25 * REFTIMES_PER_MILLISEC;
	REFERENCE_TIME m_hnsActualDuration;
	UINT32 m_bufferFrameCount;
	bool m_started;
	bool m_buffer_open;
	UINT32 m_open_buffer_frames;

	Pimpl()
		: m_event(INVALID_HANDLE_VALUE)
		, m_pEnumerator(nullptr)
		, m_pDevice(nullptr)
		, m_pAudioClient(nullptr)
		, m_pRenderClient(nullptr)
		, m_pwfx(nullptr)
		, m_started(false)
		, m_buffer_open(false)
		, m_open_buffer_frames(0)
	{
	}

	~Pimpl()
	{
		m_buffer_open = false;
		m_open_buffer_frames = 0;

		if (m_started)
		{
			BBOX_ASSERT(m_pAudioClient);

			m_pAudioClient->Stop();
		}

		if (m_pRenderClient != nullptr)
		{
			m_pRenderClient->Release();
			m_pRenderClient = nullptr;
		}

		if (m_pAudioClient != nullptr)
		{
			m_pAudioClient->Release();
			m_pAudioClient = nullptr;
		}

		if (m_pDevice != nullptr)
		{
			m_pDevice->Release();
			m_pDevice = nullptr;
		}

		if (m_pEnumerator != nullptr)
		{
			m_pEnumerator->Release();
			m_pEnumerator = nullptr;
		}

		if (m_pwfx != nullptr)
		{
			CoTaskMemFree(m_pwfx);
			m_pwfx = nullptr;
		}

		if ((m_event != INVALID_HANDLE_VALUE)
			&& (m_event != NULL))
		{
			::CloseHandle(m_event);
		}
	}

	bbox::Error OpenAndStart()
	{
		m_event = ::CreateEvent(nullptr, false, false, nullptr);
		if (m_event == NULL)
			return bbox::Error::Win32_GetLastError();

		HRESULT hr;

		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&m_pEnumerator);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		hr = m_pEnumerator->GetDefaultAudioEndpoint(
				eRender, eConsole, &m_pDevice);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		hr = m_pDevice->Activate(
				IID_IAudioClient, CLSCTX_ALL,
				NULL, (void**)&m_pAudioClient);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		hr = m_pAudioClient->GetMixFormat(&m_pwfx);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		if (m_pwfx->wFormatTag != WAVE_FORMAT_EXTENSIBLE)
		{
			// TODO
			return std::errc::connection_refused;
		}

		{
			WAVEFORMATEXTENSIBLE *wfe = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(m_pwfx);

			if (wfe->Format.nChannels != 2)
			{
				return std::errc::connection_refused;
			}

			if (wfe->SubFormat != KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
			{
				return std::errc::connection_refused;
			}
		}

		hr = m_pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			m_hnsRequestedDuration,
			0,
			m_pwfx,
			NULL);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		hr = m_pAudioClient->SetEventHandle(m_event);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		// Get the actual size of the allocated buffer.
		hr = m_pAudioClient->GetBufferSize(&m_bufferFrameCount);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		hr = m_pAudioClient->GetService(
				IID_IAudioRenderClient,
				(void**)&m_pRenderClient);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		// Grab the entire buffer for the initial fill operation.
		BYTE *pData;
		hr = m_pRenderClient->GetBuffer(m_bufferFrameCount, &pData);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		// Fill with zeros
		memset(pData, 0, m_bufferFrameCount);

		hr = m_pRenderClient->ReleaseBuffer(m_bufferFrameCount, 0);

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		// Calculate the actual duration of the allocated buffer.
		m_hnsActualDuration = REFERENCE_TIME(
			double(REFTIMES_PER_SEC)
			* m_bufferFrameCount
			/ m_pwfx->nSamplesPerSec);

		hr = m_pAudioClient->Start();  // Start playing.

		if (FAILED(hr))
			return bbox::Error::Win32_FromHRESULT(hr);

		m_started = true;
		return bbox::Error();
	}

};

WasapiRenderer::WasapiRenderer()
{
}

WasapiRenderer::~WasapiRenderer()
{
}


HANDLE WasapiRenderer::GetEventHandle()
{
	BBOX_ASSERT(m_pimpl);
	return m_pimpl->m_event;
}


bbox::Error WasapiRenderer::OpenAndStart()
{
	m_pimpl.reset();

	m_pimpl = std::make_unique<Pimpl>();

	bbox::Error error = m_pimpl->OpenAndStart();

	if (error)
	{
		m_pimpl.reset();
	}

	return error;
}

void WasapiRenderer::StopAndClose()
{
	m_pimpl.reset();
}

size_t WasapiRenderer::GetSampleRate()
{
	BBOX_ASSERT(m_pimpl);
	BBOX_ASSERT(m_pimpl->m_started);

	return m_pimpl->m_pwfx->nSamplesPerSec;
}

bbox::Error WasapiRenderer::OpenRenderBuffer(float *&buffer_ptr, size_t &buffer_frames)
{
	buffer_ptr = nullptr;
	buffer_frames = 0;

	BBOX_ASSERT(m_pimpl);
	BBOX_ASSERT(m_pimpl->m_started);
	BBOX_ASSERT(!m_pimpl->m_buffer_open);

	HRESULT hr;
	UINT32 numFramesPadding;
	BYTE *pData;

	// See how much buffer space is available.
	hr = m_pimpl->m_pAudioClient->GetCurrentPadding(&numFramesPadding);

	if (FAILED(hr))
		return bbox::Error::Win32_FromHRESULT(hr);

	m_pimpl->m_open_buffer_frames = m_pimpl->m_bufferFrameCount - numFramesPadding;

	// Grab all the available space in the shared buffer.
	hr = m_pimpl->m_pRenderClient->GetBuffer(m_pimpl->m_open_buffer_frames, &pData);

	if (FAILED(hr))
		return bbox::Error::Win32_FromHRESULT(hr);

	m_pimpl->m_buffer_open = true;
	buffer_ptr = reinterpret_cast<float *>(pData);
	buffer_frames = m_pimpl->m_open_buffer_frames;

	return bbox::Error();
}

bbox::Error WasapiRenderer::CompleteRenderBuffer()
{
	BBOX_ASSERT(m_pimpl);
	BBOX_ASSERT(m_pimpl->m_started);
	BBOX_ASSERT(m_pimpl->m_buffer_open);

	m_pimpl->m_buffer_open = false;

	HRESULT hr = m_pimpl->m_pRenderClient->ReleaseBuffer(m_pimpl->m_open_buffer_frames, 0);

	if (FAILED(hr))
		return bbox::Error::Win32_FromHRESULT(hr);

	return bbox::Error();
}

} // namespace bbox::audio::windows
} // namespace bbox::audio
} // namespace bbox
