#include "BlendState.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "RHIDevice.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

BlendFactor BlendState::ToFactor(const std::string& string)
{
	if (string == "ONE")
		return BLEND_ONE;
	else if (string == "ZERO")
		return BLEND_ZERO;
	else if (string == "SRC_ALPHA")
		return BLEND_SRC_ALPHA;
	else if (string == "INV_SRC_ALPHA")
		return BLEND_INV_SRC_ALPHA;
	ASSERT_RECOVERABLE(false, "Failed to convert string to BlendFactor!\n");
	return BLEND_ONE;
}

D3D11_BLEND BlendState::ToDxFactor(BlendFactor factor)
{
	switch (factor)
	{
	case BLEND_ONE:
		return D3D11_BLEND_ONE;
	case BLEND_ZERO:
		return D3D11_BLEND_ZERO;
	case BLEND_SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;
	case BLEND_INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
	//case BLEND_ONE_MINUS_SRC_ALPHA:
	//	return D3D11_BLEND_ONE_MINUS_ALPHA;
	default:
		return D3D11_BLEND_ONE;
	}
}

BlendState::BlendState(RHIDevice *owner, bool enabled, BlendFactor srcFactor, BlendFactor destFactor)
	: m_device(owner)
	, m_dxState(nullptr)
{
	D3D11_BLEND_DESC desc;
	MemSetZero(&desc);
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = enabled;

	//Color Blending
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = ToDxFactor(srcFactor);
	desc.RenderTarget[0].DestBlend = ToDxFactor(destFactor);

	//Alpha Blending
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//create
	ID3D11Device* dxDevice = m_device->m_dxDevice;

	HRESULT hr = dxDevice->CreateBlendState(&desc, &m_dxState);

	if (FAILED(hr))
	{
		//ASSERT_OR_DIE()
		m_dxState = nullptr;
	}
}
