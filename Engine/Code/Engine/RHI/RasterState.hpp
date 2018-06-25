#pragma once


#include "DX11.hpp"

class RHIDevice;

enum FillMode
{
	FILLMODE_SOLID,
	FILLMODE_WIRE
};

enum CullMode
{
	CULLMODE_BACK,
	CULLMODE_FRONT,
	CULLMODE_NONE
};

class RasterState 
{
public:
	RasterState( RHIDevice *owner, CullMode cullMode = CULLMODE_BACK, FillMode fillMode = FILLMODE_SOLID, bool frontCounterClockwise = true);
	~RasterState();
      
	inline bool is_valid() const { return (nullptr != dx_state); }
	static D3D11_FILL_MODE ToDxFillMode(FillMode fillMode);
	static D3D11_CULL_MODE ToDxCullMode(CullMode cullMode);

public:
	RHIDevice *device;
	ID3D11RasterizerState *dx_state;

	CullMode m_cullMode;
	FillMode m_fillMode;
};

