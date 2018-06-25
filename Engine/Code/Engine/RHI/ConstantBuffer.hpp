#pragma once

#include "DX11.hpp"

class RHIDevice;
class RHIDeviceContext;

class ConstantBuffer 
{
public:
    ConstantBuffer( RHIDevice *owner, 
        void const *buffer, 
        size_t const bufferSize );
    ~ConstantBuffer();

    bool Update( RHIDeviceContext *context, void const *buffer );

    inline bool IsValid() const { return (m_dxBuffer != nullptr); }

public:
    ID3D11Buffer *m_dxBuffer;
    size_t m_bufferSize;
}; 
