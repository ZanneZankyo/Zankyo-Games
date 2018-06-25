#pragma once
#include "DX11.hpp"
#include "RHITypes.hpp"
#include "Engine\Math\Vertex3.hpp"

class IndexBuffer
{
public:
	IndexBuffer(
		RHIDevice *owner,
		uint32_t const *indices,
		unsigned int const numOfIndices
	);
	~IndexBuffer();

	bool IsValid() const;

public:

	ID3D11Buffer *m_dxBuffer;
	unsigned int m_numOfIndices;
};

inline bool IndexBuffer::IsValid() const
{
	return (m_dxBuffer != nullptr);
}