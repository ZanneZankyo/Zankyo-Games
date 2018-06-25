#include "ComputeShader.hpp"
#include "RHIDevice.hpp"
#include "Texture2D.hpp"

ComputeShader::ComputeShader(RHIDevice* device)
	: m_device(device)
	, m_dxComputeShader(nullptr)
{}

ComputeShader::ComputeShader(RHIDevice* device, ID3D11ComputeShader *cs)
	: m_device(device)
	, m_dxComputeShader(cs)
{

}

ComputeShader::~ComputeShader()
{
	Destroy();
}

void ComputeShader::Destroy()
{
	m_dxComputeShader->Release();
}


ComputeJob::ComputeJob()
	: m_gridSize(1, 1, 1)
	, m_shader(nullptr)
	, m_tex2dIndex(0)
	, m_tex2dRes(nullptr)
{}

ComputeJob::ComputeJob(ComputeShader* shader)
	: ComputeJob()
{
	SetShader(shader);
}

void ComputeJob::SetShader(ComputeShader *cs)
{
	m_shader = cs;
}

void ComputeJob::SetGridDimensions(uint x, uint y, uint z /*= 1*/)
{
	m_gridSize.x = x;
	m_gridSize.y = y;
	m_gridSize.z = z;
}

void ComputeJob::SetResource(uint idx, Texture2D* res)
{
	UNUSED(idx); //#TODO Resource database for this job
	m_tex2dRes = res;
	SetGridDimensions(m_tex2dRes->m_width / 16, m_tex2dRes->m_height / 16);
}
