#include "DepthStencilState.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "RHIDevice.hpp"
#include "DX11.hpp"

DepthStencilState::DepthStencilState(RHIDevice *owner, const DepthStencilDesc &desc)
	: m_device(owner)
	, m_dxState(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC dxdesc;
	MemSetZero(&dxdesc);

	dxdesc.DepthEnable = desc.depthWritingEnabled || desc.depthTestEnabled;
	dxdesc.DepthWriteMask = desc.depthWritingEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dxdesc.DepthFunc = desc.depthTestEnabled ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;
	dxdesc.StencilEnable = FALSE;

	HRESULT result = m_device->m_dxDevice->CreateDepthStencilState(&dxdesc, &m_dxState);
	UNUSED(result); // ASSERT
}

DepthStencilState::~DepthStencilState()
{
	DX_SAFE_RELEASE(m_dxState);
}
