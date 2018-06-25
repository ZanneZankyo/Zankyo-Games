#pragma once
#include "RHIDevice.hpp"
#include "DX11.hpp"
#include "Texture2D.hpp"
#include "Engine\Core\RGBA.hpp"
#include "DepthStencilState.hpp"
#include "RHITypes.hpp"

class RHIDeviceContext
{
public:
	RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx = nullptr);
	~RHIDeviceContext();

	// For an immediate/display context, we need the window we're rendering to
	// and the swapchain that manages the render target
	void ClearState();  // clears all rendering state
	void Flush();        // flushes all commands

	void Dispatch(ComputeJob* job);

	void ClearColorTarget(Texture2D* output, const Rgba & color);
	void ClearDepthTarget(Texture2D *output, float depth = 1.0f, uint8_t stencil = 0);

	void SetColorTarget(Texture2D * colorTarget, Texture2D * depthTarget = nullptr);
	void SetRasterState(RasterState * rasterState);
	void SetBlendState(BlendState* blendState);
	void SetViewports(unsigned int x, unsigned int y, unsigned int width, unsigned int height);


	void SetShader(ShaderProgram * shader);
	void SetPrimitiveTopology(PrimitiveType topology);
	void SetVertexBuffer(VertexBuffer * vbo);
	void SetRenderTarget(Texture2D * current_target);
	void SetShaderResources(unsigned int textureIndex, Texture2D * texture);

	void SetComputeShader(ComputeShader * computeShader);
	void SetUnorderedAccessViews(unsigned int slotIndex, Texture2D * uav);

	void Draw(int numOfVertices, int StartVerticesLocation);
	void DrawIndexed(int numOfIndices, int StartIndicesLocation, int baseVertexLocation);
	void SetDepthStencilState(DepthStencilState * m_depthStencilState);

	void SetStructuredBuffer(unsigned int bind_point, StructuredBuffer *buffer);
	void SetComputeConstantBuffer(unsigned int slotIndex, ConstantBuffer* constantBuffer);
public:
	RHIDevice *m_device; 
	ID3D11DeviceContext *m_dxContext;
	
	
	void SetIndexBuffer(IndexBuffer * indexedBuffer);
	void SetConstantBuffer(unsigned slotIndex, ConstantBuffer * constantBuffer);
	void SetSampler(Sampler * sampler, unsigned int textureIndex);
	void SetComputeSampler(Sampler* sampler, unsigned int textureIndex);
};

inline RHIDeviceContext::RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx)
	: m_device(owner)
	, m_dxContext(ctx)
{}

inline RHIDeviceContext::~RHIDeviceContext()
{
}
