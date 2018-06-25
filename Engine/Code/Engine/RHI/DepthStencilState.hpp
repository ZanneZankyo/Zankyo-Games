#pragma once

#include "DX11.hpp"
#include "RHITypes.hpp"

struct DepthStencilDesc
{
	bool depthWritingEnabled; // if writing a pixel - also write a depth
	bool depthTestEnabled;    // only write a pixel if depth written is less than or equal current current depth
};

class DepthStencilState
{
public:
	DepthStencilState(RHIDevice* owner, const DepthStencilDesc& desc);
	~DepthStencilState();

	bool IsValid() const;

	RHIDevice *m_device;
	ID3D11DepthStencilState * m_dxState;
};

inline bool DepthStencilState::IsValid() const
{
	return m_device != nullptr;
}