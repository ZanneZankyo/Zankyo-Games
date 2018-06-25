#pragma once
#include "RHITypes.hpp"
#include "DX11.hpp"
#include <string>

class RHIDevice;

enum BlendFactor : unsigned int
{
	BLEND_ONE,
	BLEND_ZERO,
	BLEND_SRC_ALPHA,
	BLEND_INV_SRC_ALPHA,
	//BLEND_ONE_MINUS_SRC_ALPHA,
};

enum BlendMode : unsigned int
{
	BLEND_NONE,
	BLEND_ADDITIVE,
	BLEND_B
};

class BlendState
{
public:
	BlendState(RHIDevice *owner, bool enabled = true, BlendFactor srcFactor = BLEND_ONE, BlendFactor destFactor = BLEND_ZERO);
	~BlendState();

	bool IsValid() const;
	static BlendFactor ToFactor(const std::string& string);
	static D3D11_BLEND ToDxFactor(BlendFactor factor);
public:
	RHIDevice *m_device;
	ID3D11BlendState* m_dxState;
};

inline bool BlendState::IsValid() const
{
	return m_dxState != nullptr;
}

inline BlendState::~BlendState()
{}