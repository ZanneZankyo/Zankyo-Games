#include "ShaderProgram.hpp"
#include "RHIDevice.hpp"
#include <stddef.h>
#include "Engine\Math\Vertex3.hpp"

/*
ShaderProgram::ShaderProgram(RHIDevice * owner, char const * filename)
{
	
}*/

ShaderProgram::ShaderProgram(RHIDevice * owner, char const * filename)
{
	*this = *owner->CreateShaderProgramFromHlslFile(filename);
}

ShaderProgram::ShaderProgram(RHIDevice * device, ID3D11VertexShader * vs, ID3D11PixelShader * fs, ID3DBlob * vs_bytecode, ID3DBlob * fs_bytecode)
	: dx_vertex_shader(vs)
	, dx_fragment_shader(fs)
	, vs_byte_code(vs_bytecode)
	, fs_byte_code(fs_bytecode)
{
	dx_input_layout = nullptr;
	CreateInputLayout(device);
}

ShaderProgram::~ShaderProgram()
{
	dx_vertex_shader->Release();
	dx_fragment_shader->Release();
	vs_byte_code->Release();
	fs_byte_code->Release();
}

void ShaderProgram::CreateInputLayout(RHIDevice * device)
{
	D3D11_INPUT_ELEMENT_DESC desc[7];
	memset(desc, 0, sizeof(desc));

	// POSITION
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlot = 0U;
	desc[0].AlignedByteOffset = offsetof(Vertex3, position);
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[0].InstanceDataStepRate = 0U;

	// UV
	desc[1].SemanticName = "UV";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[1].InputSlot = 0U;
	desc[1].AlignedByteOffset = offsetof(Vertex3, texCoords);
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].InstanceDataStepRate = 0U;

	// COLOR
	desc[2].SemanticName = "COLOR";
	desc[2].SemanticIndex = 0;
	desc[2].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc[2].InputSlot = 0U;
	desc[2].AlignedByteOffset = offsetof(Vertex3, color);
	desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[2].InstanceDataStepRate = 0U;

	// NORMAL
	desc[3].SemanticName = "NORMAL";
	desc[3].SemanticIndex = 0;
	desc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[3].InputSlot = 0U;
	desc[3].AlignedByteOffset = offsetof(Vertex3, normal);
	desc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[3].InstanceDataStepRate = 0U;

	// TANGENT
	desc[4].SemanticName = "TANGENT";
	desc[4].SemanticIndex = 0;
	desc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[4].InputSlot = 0U;
	desc[4].AlignedByteOffset = offsetof(Vertex3, tangent);
	desc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[4].InstanceDataStepRate = 0U;

	// BONE_INDICES
	desc[5].SemanticName = "BONE_INDICES";
	desc[5].SemanticIndex = 0;
	desc[5].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	desc[5].InputSlot = 0U;
	desc[5].AlignedByteOffset = offsetof(Vertex3, boneIndices);
	desc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[5].InstanceDataStepRate = 0U;

	// BONE_WEIGHTS
	desc[6].SemanticName = "BONE_WEIGHTS";
	desc[6].SemanticIndex = 0;
	desc[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[6].InputSlot = 0U;
	desc[6].AlignedByteOffset = offsetof(Vertex3, boneWeights);
	desc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[6].InstanceDataStepRate = 0U;

	device->m_dxDevice->CreateInputLayout(desc,
		ARRAYSIZE(desc),
		vs_byte_code->GetBufferPointer(),
		vs_byte_code->GetBufferSize(),
		&dx_input_layout);
}