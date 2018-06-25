#pragma once

#include "DX11.hpp"

#include "Engine/Math/Vertex3.hpp"
#include "RHITypes.hpp"

class RHIDevice;

class VertexBuffer 
{
public:
	VertexBuffer(
		RHIDevice *owner,
		Vertex3 const *vertices,
		unsigned int const numOfVertices
	);
	~VertexBuffer(); 

	bool IsValid() const;

	public:

	ID3D11Buffer *m_dxBuffer;
	unsigned int m_numOfVertices;
}; 

inline bool VertexBuffer::IsValid() const
{
	return (m_dxBuffer != nullptr);
}