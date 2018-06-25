#pragma once
#include <d3d11.h>
#include <vector>
//#include "Engine\RHI\Texture2D.hpp"
#include "Engine\RHI\RHITypes.hpp"

class VideoDecoder
{
private:
	ID3D11VideoDevice* m_dxVideoDevice;
	ID3D11VideoDecoder* m_dxDecoder;
	std::vector<Texture2D*> m_frames;
	RHIDevice* m_device;
public:
	VideoDecoder(RHIDevice* device);
public:
	static uint16_t s_videoDecoderSampleWidth;
	static uint16_t s_videoDecoderSampleHeight;
};