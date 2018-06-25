#pragma once
#include "DX11.hpp"
#include "..\Core\EngineBase.hpp"
#include "RHIDevice.hpp"

class RHIDevice;

enum FilterMode
{
	FILTER_POINT,
	FILTER_LINEAR,
};

class Sampler
{
public:
	Sampler(RHIDevice *device);
	Sampler(RHIDevice *device, FilterMode minFilter, FilterMode magFilter);
	~Sampler();

	bool CreateSampler(FilterMode minFilter, FilterMode magFilter);
	bool IsVaild();

public:
	ID3D11SamplerState* m_dxSampler;
	RHIDevice* m_device;
private:
	D3D11_FILTER DXGetFIlterMode(FilterMode minFilter, FilterMode magFilter);
};



inline bool Sampler::IsVaild()
{
	return m_dxSampler != nullptr;
}