#include "RHIDevice.hpp"
#include "ConstantBuffer.hpp"
#include "RHIDeviceContext.hpp"

ConstantBuffer::ConstantBuffer( RHIDevice *device, 
   void const *buffer, 
   size_t const size )
{
   m_bufferSize = size;

   // First, describe the buffer
   D3D11_BUFFER_DESC vb_desc;
   memset( &vb_desc, 0, sizeof(vb_desc) );

   vb_desc.ByteWidth = (unsigned int)m_bufferSize;                      // How much data are we putting into this buffer
   vb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // What can we bind this data as (in this case, only vertex data)
   vb_desc.Usage = D3D11_USAGE_DYNAMIC;                  // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
                                                         // for limitations/strenghts of each, see;  
                                                         //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
   vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   vb_desc.StructureByteStride = 0;                      // How large is each element in this buffer

   // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
   D3D11_SUBRESOURCE_DATA initial_data;
   memset( &initial_data, 0, sizeof(initial_data) );
   initial_data.pSysMem = buffer;

   // Finally create the vertex buffer
   m_dxBuffer = nullptr;
   device->m_dxDevice->CreateBuffer( &vb_desc, &initial_data, &m_dxBuffer );
}

//------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
   DX_SAFE_RELEASE(m_dxBuffer);
}

//------------------------------------------------------------------------
bool ConstantBuffer::Update( RHIDeviceContext *context, void const *buffer )
{
   D3D11_MAPPED_SUBRESOURCE resource;

   ID3D11DeviceContext *dx_context = context->m_dxContext;
   if (SUCCEEDED(dx_context->Map( m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource ))) 
   {
      memcpy( resource.pData, buffer, m_bufferSize );
      dx_context->Unmap( m_dxBuffer, 0 );

      return true;
   }

   // ASSERT?
   return false;
}