#pragma once

#include "DX11.hpp"
#include "RHITypes.hpp"

class StructuredBuffer
{
public:
	// Note, it takes an object size, and object_count instead
	// of the arrays total size, as object_size matters in 
	// creation!
	StructuredBuffer(RHIDevice *owner,
		void const *buffer,
		unsigned int object_size,
		unsigned int object_count);
	~StructuredBuffer();

	// New for a structured buffer!
	bool CreateViews();

	bool Update(RHIDeviceContext *context, void const *buffer);

	inline bool IsValid() const { return (m_dxBuffer != nullptr); }

public:
	ID3D11Buffer *m_dxBuffer;
	ID3D11ShaderResourceView *m_dxSrv;
	RHIDevice *m_device;

	unsigned int m_objCount;
	unsigned int m_objSize;
	size_t m_bufferSize;
};