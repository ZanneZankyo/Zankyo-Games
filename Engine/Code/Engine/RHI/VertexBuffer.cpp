#include "VertexBuffer.hpp"
#include "RHIDevice.hpp"

VertexBuffer::VertexBuffer( RHIDevice *device, 
	Vertex3 const *vertices,
   unsigned int const numOfVertices )
	: m_numOfVertices(numOfVertices)
{
   // First, describe the buffer
   D3D11_BUFFER_DESC vbDesc;
   memset( &vbDesc, 0, sizeof(vbDesc) );

   vbDesc.ByteWidth = sizeof(Vertex3) * numOfVertices;  // How much data are we putting into this buffer
   vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
   vbDesc.Usage = D3D11_USAGE_IMMUTABLE;                // Hint on how this memory is used (in this case, it is immutable, or constant - can't be changed)
                                                         // for limitations/strenghts of each, see;  
                                                         //    https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
   vbDesc.StructureByteStride = sizeof(Vertex3);       // How large is each element in this buffer

   // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
   D3D11_SUBRESOURCE_DATA initial_data;
   memset( &initial_data, 0, sizeof(initial_data) );
   initial_data.pSysMem = vertices;

   // Finally create the vertex buffer
   m_dxBuffer = nullptr;
   device->m_dxDevice->CreateBuffer( &vbDesc, &initial_data, &m_dxBuffer );
}

//------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
   DX_SAFE_RELEASE(m_dxBuffer);
}
