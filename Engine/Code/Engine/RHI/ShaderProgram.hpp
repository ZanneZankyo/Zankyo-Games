#pragma once
//#include "Engine\RHI\RHIDevice.hpp"
#include "Engine\RHI\DX11.hpp"

class VertexBuffer;
class RHIDevice;

enum eShaderStage
{
	SHADER_VERTEX,    // maps to target "vs_5_0", or Vertex Shader Model 5 (currently latest)
	SHADER_FRAGMENT,  // maps to target "ps_5_0", or Pixel Shader Model 5 (currently latest)
};

class ShaderProgram
{
public:
	ShaderProgram(RHIDevice *owner, char const *filename);
	ShaderProgram(RHIDevice *device,
		ID3D11VertexShader *vs, ID3D11PixelShader *fs,
		ID3DBlob *vs_bytecode, ID3DBlob *fs_bytecode);
	~ShaderProgram();

	void CreateInputLayout(RHIDevice* device);
	

	// feel free to add in more methods to help with the creation process if you want.

	inline bool IsValid() const { return (dx_vertex_shader != nullptr) && (dx_fragment_shader != nullptr); }

public:
	// All the steps to this (could be split out to a ShaderStage)
	ID3D11VertexShader *dx_vertex_shader;
	ID3D11PixelShader *dx_fragment_shader;

	// Input Layout - for now, we're going 
	// to assume every shader makes their own
	// for simplicity, but honestly you should make these
	// based on need
	ID3D11InputLayout *dx_input_layout;

	// [OPTIONAL] ByteCode - only need to keep it around if using Reflection
	ID3DBlob *vs_byte_code;
	ID3DBlob *fs_byte_code;
};