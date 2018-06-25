#include "RHIDevice.hpp"
#include "Texture2D.hpp"
#include "RHIDeviceContext.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include <vector>
#include "Engine\File\Persistence.hpp"
#include <d3dcommon.h>
#include "VertexBuffer.hpp"
#include "RHIInstance.hpp"
#include "RHITypes.hpp"
#include "Engine\Core\Console.hpp"
#include "IndexBuffer.hpp"
#include "ComputeShader.hpp"

static ID3DBlob* CompileHLSLToShaderBlob(char const *opt_filename,  // optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target)                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile(source_code,
		source_code_size,                   // plain text source code
		opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compile_flags,                      // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors);                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*)errors->GetBufferPointer();
			DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
				opt_filename,
				error_string);
			DX_SAFE_RELEASE(errors);
		}
	}

	// will be nullptr if it failed to compile
	return code;
}

RHIDeviceContext* RHIDevice::GetImmediateContext()
{
	return m_immediateContext;
}

ShaderProgram * RHIDevice::CreateShaderProgramFromHlslFile(const std::string& filename)
{

	std::vector<unsigned char> srcCode;
	Persistence::LoadBinaryFileToBuffer(filename, srcCode);

	if (srcCode.empty()) {
		Console::Log("Shader " + filename + " failed to load.", Rgba::RED);
		//ASSERT_OR_DIE( 0, "Shader File "+ filename +" not found." );
		return nullptr;
	}

	ID3DBlob *vsByteCode = nullptr;
	ID3DBlob *fsByteCode = nullptr;

	vsByteCode = CompileHLSLToShaderBlob(filename.c_str(), srcCode.data(), srcCode.size(), "VertexFunction", "vs_5_0");
	fsByteCode = CompileHLSLToShaderBlob(filename.c_str(), srcCode.data(), srcCode.size(), "FragmentFunction", "ps_5_0");
	//delete srcCode;

	if ((vsByteCode != nullptr)
		&& (fsByteCode != nullptr)) {

		ID3D11VertexShader *vs = nullptr;
		ID3D11PixelShader *fs = nullptr;

		m_dxDevice->CreateVertexShader(vsByteCode->GetBufferPointer(),
			vsByteCode->GetBufferSize(),
			nullptr,
			&vs);

		m_dxDevice->CreatePixelShader(fsByteCode->GetBufferPointer(),
			fsByteCode->GetBufferSize(),
			nullptr,
			&fs);

		ShaderProgram *program = new ShaderProgram(this, vs, fs, vsByteCode, fsByteCode);
		return program;
	}
	Console::Log("Shader " + filename + " failed to compile.", Rgba::RED);
	return nullptr;
}

ComputeShader* RHIDevice::CreateComputeShaderFromHlslFile(const std::string& filename)
{
	std::vector<unsigned char> srcCode;
	Persistence::LoadBinaryFileToBuffer(filename, srcCode);

	if (srcCode.empty()) {
		Console::Log("Compute Shader " + filename + " failed to load.", Rgba::RED);
		//ASSERT_OR_DIE( 0, "Shader File "+ filename +" not found." );
		return nullptr;
	}

	ID3DBlob *csByteCode = nullptr;

	csByteCode = CompileHLSLToShaderBlob(filename.c_str(), srcCode.data(), srcCode.size(), "ComputeFunction", "cs_5_0");
	
	if (!csByteCode)
	{
		Console::Log("Compute Shader " + filename + " failed to load.", Rgba::RED);
		return nullptr;
	}

	ID3D11ComputeShader *cs = nullptr;
	HRESULT hr = m_dxDevice->CreateComputeShader(csByteCode->GetBufferPointer(),
		csByteCode->GetBufferSize(),
		nullptr,
		&cs);
	if(SUCCEEDED(hr))
		return new ComputeShader(this, cs);
	return nullptr;
}

void RHIDevice::CompileShaderCode()
{
	
}

VertexBuffer * RHIDevice::CreateVertexBuffer(Vertex3 const * vertices, unsigned int const vertex_count, BufferUsage const usage)
{
	UNUSED(usage);
	VertexBuffer *buffer = new VertexBuffer(this, vertices, vertex_count);
	return buffer;
}

IndexBuffer * RHIDevice::CreateIndexBuffer(uint32_t const * indices, unsigned int const index_count, BufferUsage const usage)
{
	UNUSED(usage);
	IndexBuffer *buffer = new IndexBuffer(this, indices, index_count);
	return buffer;
}

/*
Texture2D * RHIDevice::CreateTextureFromImage(const Image & image)
{
	return nullptr;
}*/

/*
RHIDevice::RHIDevice()
	: immediate_context(new RHIDeviceContext(this))
	, dx_device(nullptr)
{}*/

RHIDevice::RHIDevice(RHIInstance * owner, ID3D11Device * dxd)
	: m_immediateContext(new RHIDeviceContext(this))
	, m_dxDevice(dxd)
	, instance(owner)
{}

RHIDevice::~RHIDevice()
{
	SAFE_DELETE(m_immediateContext);
	DX_SAFE_RELEASE(m_dxDevice);
}

