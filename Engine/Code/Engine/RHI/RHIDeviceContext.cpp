#include "RHIDeviceContext.hpp"
#include "DX11.hpp"
#include "RasterState.hpp"
#include "BlendState.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "StructuredBuffer.hpp"
#include "ConstantBuffer.hpp"
#include "ComputeShader.hpp"
#include "Sampler.hpp"

void RHIDeviceContext::Dispatch(ComputeJob* job)
{
	SetUnorderedAccessViews(0,job->m_tex2dRes);
	SetComputeShader(job->m_shader);
	m_dxContext->Dispatch(job->m_gridSize.x, job->m_gridSize.y, job->m_gridSize.z);
}

void RHIDeviceContext::ClearColorTarget(Texture2D* output, const Rgba & color)
{
	float scale = 1.f / 255.f;
	float clear_color[4] = { color.red * scale, color.green * scale, color.blue * scale, color.alpha * scale };
	m_dxContext->ClearRenderTargetView(output->m_dxRtv, clear_color);
	//DX_SAFE_RELEASE(output->m_dxRtv);
}

void RHIDeviceContext::ClearDepthTarget(Texture2D *output, float depth /*= 1.0f*/, uint8_t stencil /*= 0*/)
{
	if (output != nullptr) {
		m_dxContext->ClearDepthStencilView(output->m_dxDsv,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			depth, stencil);
	}
}

void RHIDeviceContext::SetRasterState(RasterState * rasterState)
{
	m_dxContext->RSSetState(rasterState->dx_state);
}

void RHIDeviceContext::SetBlendState(BlendState* blendState)
{
	float constant[] = { 1.f,1.f,1.f,1.f };
	m_dxContext->OMSetBlendState(blendState->m_dxState, constant, 0xffffffff);
}

void RHIDeviceContext::SetViewports(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)

	m_dxContext->RSSetViewports(1, &viewport);
}

void RHIDeviceContext::SetShader(ShaderProgram * shader)
{
	m_dxContext->VSSetShader(shader->dx_vertex_shader, nullptr, 0U);
	m_dxContext->PSSetShader(shader->dx_fragment_shader, nullptr, 0U);
	m_dxContext->IASetInputLayout(shader->dx_input_layout);
}

void RHIDeviceContext::SetComputeShader(ComputeShader * computeShader)
{
	m_dxContext->CSSetShader(computeShader->m_dxComputeShader, nullptr, 0U);
}

void RHIDeviceContext::SetUnorderedAccessViews(unsigned int slotIndex, Texture2D * uav)
{
	if (!uav)
	{
		ID3D11UnorderedAccessView* view = nullptr;
		m_dxContext->CSSetUnorderedAccessViews(slotIndex, 1, &view, nullptr);
		return;
	}
	m_dxContext->CSSetUnorderedAccessViews(slotIndex, 1, &uav->m_dxUav, nullptr);
}

void RHIDeviceContext::SetPrimitiveTopology(PrimitiveType topology)
{
	D3D11_PRIMITIVE_TOPOLOGY d3d_prim;
	switch (topology) {
	case PRIMITIVE_LINES:
		d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PRIMITIVE_TRIANGLES:
		d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
	m_dxContext->IASetPrimitiveTopology(d3d_prim);
}

void RHIDeviceContext::SetVertexBuffer(VertexBuffer * vbo)
{
	unsigned int stride = sizeof(Vertex3);
	unsigned int offsets = 0;

	m_dxContext->IASetVertexBuffers(0, 1, &vbo->m_dxBuffer, &stride, &offsets);
}

void RHIDeviceContext::Draw(int numOfVertices, int StartVerticesLocation)
{
	m_dxContext->Draw(numOfVertices, StartVerticesLocation);
}

void RHIDeviceContext::DrawIndexed(int numOfIndices, int StartIndicesLocation, int baseVertexLocation)
{
	m_dxContext->DrawIndexed(numOfIndices, StartIndicesLocation, baseVertexLocation);
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState * depthStencilState)
{
	m_dxContext->OMSetDepthStencilState(depthStencilState->m_dxState, 0U);
}

void RHIDeviceContext::SetStructuredBuffer(unsigned int bind_point, StructuredBuffer *buffer)
{
	// todo: check for null, and unbind if so.
	if (!buffer)
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		m_dxContext->VSSetShaderResources(bind_point, 1, &nullSRV);
		m_dxContext->PSSetShaderResources(bind_point, 1, &nullSRV);
		return;
	}
	m_dxContext->VSSetShaderResources(bind_point, 1, &buffer->m_dxSrv);
	m_dxContext->PSSetShaderResources(bind_point, 1, &buffer->m_dxSrv);
}

void RHIDeviceContext::SetComputeConstantBuffer(unsigned int slotIndex, ConstantBuffer* constantBuffer)
{
	if (!constantBuffer)
	{
		ID3D11Buffer* nullBuffer = nullptr;
		m_dxContext->CSSetConstantBuffers(slotIndex, 1, &nullBuffer);
		return;
	}
	m_dxContext->CSSetConstantBuffers(slotIndex, 1, &constantBuffer->m_dxBuffer);
}

void RHIDeviceContext::SetIndexBuffer(IndexBuffer * indexedBuffer)
{
	m_dxContext->IASetIndexBuffer(indexedBuffer->m_dxBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void RHIDeviceContext::SetConstantBuffer(unsigned slotIndex, ConstantBuffer * constantBuffer)
{
	if (!constantBuffer)
	{
		ID3D11Buffer* nullBuffer = nullptr;
		m_dxContext->VSSetConstantBuffers(slotIndex, 1, &nullBuffer);
		m_dxContext->PSSetConstantBuffers(slotIndex, 1, &nullBuffer);
		return;
	}
	m_dxContext->VSSetConstantBuffers(slotIndex, 1, &constantBuffer->m_dxBuffer);
	m_dxContext->PSSetConstantBuffers(slotIndex, 1, &constantBuffer->m_dxBuffer);
}

void RHIDeviceContext::SetSampler(Sampler * sampler, unsigned int textureIndex)
{
	m_dxContext->VSSetSamplers(textureIndex, 1, &sampler->m_dxSampler);
	m_dxContext->PSSetSamplers(textureIndex, 1, &sampler->m_dxSampler);
}

void RHIDeviceContext::SetComputeSampler(Sampler* sampler, unsigned int textureIndex)
{
	m_dxContext->CSSetSamplers(textureIndex, 1, &sampler->m_dxSampler);
}

void RHIDeviceContext::SetRenderTarget(Texture2D * current_target)
{
	if(current_target)
		m_dxContext->OMSetRenderTargets(1, &current_target->m_dxRtv, nullptr);
	else
		m_dxContext->OMSetRenderTargets(0, 0, 0);
}

void RHIDeviceContext::SetShaderResources(unsigned int textureIndex, Texture2D * texture)
{
	m_dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
	m_dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
}

void RHIDeviceContext::SetColorTarget(Texture2D * colorTarget, Texture2D * depthTarget)
{
	m_dxContext->OMSetRenderTargets(1,
		&colorTarget->m_dxRtv,
		depthTarget != nullptr ? depthTarget->m_dxDsv : nullptr);
}
