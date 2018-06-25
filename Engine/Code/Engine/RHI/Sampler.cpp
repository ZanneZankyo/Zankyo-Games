#include "Sampler.hpp"

Sampler::Sampler(RHIDevice * device)
	:m_device(device)
	,m_dxSampler(nullptr)
{
}

Sampler::Sampler(RHIDevice * device, FilterMode minFilter, FilterMode maxFilter)
	: Sampler(device)
{
	CreateSampler(minFilter, maxFilter);
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_dxSampler);
}

D3D11_FILTER Sampler::DXGetFIlterMode(FilterMode minFilter, FilterMode maxFilter)
{
	if (minFilter == FILTER_POINT) 
	{
		if (maxFilter == FILTER_POINT)
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		else
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else
	{
		if (maxFilter == FILTER_POINT)
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		else
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	//return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
}

bool Sampler::CreateSampler(FilterMode minFilter, FilterMode maxFilter)
{
	D3D11_SAMPLER_DESC desc;
	MemSetZero(&desc);
	desc.Filter = DXGetFIlterMode(minFilter, maxFilter);
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//desc.BorderColor = (FLOAT)0.f;
	desc.MinLOD = (FLOAT)0;
	desc.MaxLOD = (FLOAT)0;

	ID3D11Device *dxDevice = m_device->m_dxDevice;
	HRESULT hr = dxDevice->CreateSamplerState(&desc, &m_dxSampler);

	return SUCCEEDED(hr);
}