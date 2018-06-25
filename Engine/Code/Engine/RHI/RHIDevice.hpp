#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/Window.hpp"
//#include "RHIOutput.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Vertex3.hpp"
#include "ShaderProgram.hpp"
#include <string>
#include "RHITypes.hpp"


// Created from RenderInstance when initial output is created
class RHIDevice
{
public:
	RHIDevice(RHIInstance *owner, ID3D11Device *dxd);
	~RHIDevice();

	RHIDeviceContext* GetImmediateContext();//get_immediate_context();

	// [OPTIONAL] Ability to create secondary outputs
	//            Neat for screen savers, tools, or utilizing a second monitor
	RHIOutput* /*create_output*/CreateOutput(Window *window/*, eRHIOutputMode const mode*/);
	RHIOutput* /*create_output*/CreateOutput(UINT const px_width, UINT const px_height/*, eRHIOutputMode const mode*/);

	// Ability to create shaders
	ShaderProgram* CreateShaderProgramFromHlslFile(const std::string& filename);
	ComputeShader* CreateComputeShaderFromHlslFile(const std::string& filename);
	void CompileShaderCode();

	// VBOs and IBOs
	VertexBuffer* CreateVertexBuffer(Vertex3 const *vertices,
		unsigned int const vertex_count,
		BufferUsage const usage = BUFFERUSAGE_STATIC);

	IndexBuffer* CreateIndexBuffer(uint32_t const *indices,
		unsigned int const index_count,
		BufferUsage const usage = BUFFERUSAGE_STATIC);

	//Texture2D* CreateTextureFromImage(const Image& image);

public:
	// Immediate Mode Context - Created with the Device
	RHIInstance *instance;
	RHIDeviceContext *m_immediateContext;
	ID3D11Device *m_dxDevice;
};