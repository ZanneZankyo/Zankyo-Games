#include "VideoDecoder.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "RHIDevice.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

STATIC uint16_t VideoDecoder::s_videoDecoderSampleWidth = 1280;
STATIC uint16_t VideoDecoder::s_videoDecoderSampleHeight = 720;

VideoDecoder::VideoDecoder(RHIDevice* device)
	: m_device(device)
	, m_dxDecoder(nullptr)
	, m_dxVideoDevice(nullptr)
{
	HRESULT hr = device->m_dxDevice->QueryInterface(__uuidof(ID3D11VideoDevice), reinterpret_cast<void**>(&m_dxVideoDevice));
	ASSERT_OR_DIE(SUCCEEDED(hr), "DxDevice failed to query VideoDevice");
	ASSERT_OR_DIE(m_dxVideoDevice, "VideoDecoder::m_dxVideoDevice is nullptr");
	uint profileCount = m_dxVideoDevice->GetVideoDecoderProfileCount();
	ASSERT_OR_DIE(profileCount, "m_dxVideoDevice->GetVideoDecoderProfileCount() returns 0");
	GUID profile;
	m_dxVideoDevice->GetVideoDecoderProfile(0,&profile);

	D3D11_VIDEO_DECODER_DESC desc;
	desc.Guid = profile;
	desc.SampleWidth = s_videoDecoderSampleWidth;
	desc.SampleWidth = s_videoDecoderSampleHeight;
	desc.OutputFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	uint configCount = 0;
	hr = m_dxVideoDevice->GetVideoDecoderConfigCount(&desc, &configCount);
	ASSERT_OR_DIE(SUCCEEDED(hr), "m_dxVideoDevice->GetVideoDecoderConfigCount(&desc, &configCount) Failed");
	ASSERT_OR_DIE(configCount, "m_dxVideoDevice->GetVideoDecoderConfigCount(&desc, &configCount) returns 0");

	D3D11_VIDEO_DECODER_CONFIG config;
	hr = m_dxVideoDevice->GetVideoDecoderConfig(&desc, 0, &config);
	ASSERT_OR_DIE(SUCCEEDED(hr), "m_dxVideoDevice->GetVideoDecoderConfig(&desc, 0, &config) Failed");

	hr = m_dxVideoDevice->CreateVideoDecoder(&desc, &config, &m_dxDecoder);
	ASSERT_OR_DIE(SUCCEEDED(hr), "CreateVideoDecoder(&desc, &config, &m_dxDecoder) Failed");
	ASSERT_OR_DIE(m_dxDecoder, "VideoDecoder::m_dxDecoder is nullptr");
}
