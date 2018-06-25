#include "RasterState.hpp"
#include "RHIDevice.hpp"
#include "Engine\Core\EngineBase.hpp"

RasterState::RasterState(RHIDevice *owner, CullMode cullMode /*= CULLMODE_BACK*/, FillMode fillMode /*= FILLMODE_SOLID*/, bool frontCounterClockwise /*= true*/)
	: device(owner)
	, dx_state(nullptr)
	, m_cullMode(cullMode)
	, m_fillMode(fillMode)
{
   D3D11_RASTERIZER_DESC desc;
   MemSetZero( &desc );

   desc.FillMode = ToDxFillMode(fillMode); // D3D11_FILL_WIRE
   desc.CullMode = ToDxCullMode(cullMode);//D3D11_CULL_BACK;

   // Make it act like OpenGL
   desc.FrontCounterClockwise = frontCounterClockwise;

   desc.AntialiasedLineEnable = false;
   desc.DepthBias = 0;
   desc.DepthBiasClamp = 0.0f;
   desc.SlopeScaledDepthBias = 0.0f;
   desc.DepthClipEnable = true;
   desc.ScissorEnable = false;
   desc.MultisampleEnable = false;

   HRESULT result = device->m_dxDevice->CreateRasterizerState( &desc, &dx_state );
   UNUSED(result); // ASSERT
}

//------------------------------------------------------------------------
RasterState::~RasterState()
{
   DX_SAFE_RELEASE(dx_state);
}

D3D11_FILL_MODE RasterState::ToDxFillMode(FillMode fillMode)
{
	switch (fillMode)
	{
	case FILLMODE_SOLID:
		return D3D11_FILL_SOLID;
	case FILLMODE_WIRE:
		return D3D11_FILL_WIREFRAME;
	default:
		return D3D11_FILL_SOLID;
	}
}

D3D11_CULL_MODE RasterState::ToDxCullMode(CullMode cullMode)
{
	switch (cullMode)
	{
	case CULLMODE_BACK:
		return D3D11_CULL_BACK;
	case CULLMODE_FRONT:
		return D3D11_CULL_FRONT;
	case CULLMODE_NONE:
		return D3D11_CULL_NONE;
	default:
		return D3D11_CULL_BACK;
	}
}

