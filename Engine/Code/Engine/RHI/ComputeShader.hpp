#pragma once
#include "Engine\Core\EngineBase.hpp"
#include "DX11.hpp"
#include "RHITypes.hpp"
#include "..\Math\Vector2.hpp"
#include "..\Math\Vector4.hpp"
#include "..\Core\RGBA.hpp"

struct uint3
{
	int x;
	int y;
	int z;
	uint3(int px, int py, int pz)
		: x(px)
		, y(py)
		, z(pz)
	{}
};

class ComputeJob
{
public:
	ComputeJob();
	ComputeJob(ComputeShader* shader);
	ComputeJob(RHIDevice* device, const std::string& shaderName);
	//~ComputeJob();

	// Sets the shader program to use for this job;
	void SetShader(ComputeShader *cs);

	// dispach grid - how many groups do we dispatch?
	// These arguments are just forwarded to ID3D11DeviceContext::Dispatch
	// when we run ("dispatch") this job.
	void SetGridDimensions(uint x, uint y, uint z = 1);

	//void set_resource(uint idx, RHIResource res);
	void SetResource(uint idx, Texture2D* res);
	void set_sampler(uint idx, Sampler* samp);

	// Set a UA resource view (bind points use register(u#) in the sahder)
	// I use "rw" for read/write resource - since I think it reads better than UA (unordered access)
	//void set_rw_resource(uint idx, RHIResource res);


	// Sets the properties for this program by name
	// This is identical to proper blocks for Materials [optional task that was done
	// in AES].  This is not required for the assignment - but does help make the 
	// system more useable.
	//
	// If you have materials working in your engine - I recommend doing this step.  But since
	// it requires using shader reflection to have it work by property name, I recommend saving it for last if
	// you have not yet done it for a material class.
	void SetUint(char const* id, uint const &v);
	void SetFloat(char const* id, float const &v);
	void SetVector2(char const* id, Vector2 const &v);
	void SetVector4(char const* id, Vector4 const &v);
	void SetColor(char const* id, Rgba const &v);
	// etc...

public:
	ComputeShader *m_shader;

	// the grid/block size use for a dispatch
	// You can store this with the job - or pass it to your
	// RHIDeviceContext::Dispatch call - I just prefer to keep it with 
	// the job.
	uint3 m_gridSize;

	// members: left as an excercise to the reader,
	// plus they change depending on how many features you want to add or how
	// you decide to implement them....
	int m_tex2dIndex;
	Texture2D* m_tex2dRes;
};

class ComputeShader
{
public:
	// Similar to a pipeline shader - needs the device used for creation
	// and the .hlsl file to load.
	ComputeShader(RHIDevice* device);

	ComputeShader(RHIDevice* device, ID3D11ComputeShader *cs);

	// Cleans up used resources
	~ComputeShader();
	void Destroy();

	// Load a compute shader from an *.hlsl file.  Recommend 
	// starting with clear.hlsl
	bool load_file(char const *filename,
		char const *entry_point = nullptr,
		char const *defines = nullptr);

public:
	// At bare minimum, a ComputeShader just has the dx resource in it;
	// If you have a ShaderStage pipeline already in place for
	// your rendering pipeline, you could instead have a single ShaderStage 
	// here instead;
	ID3D11ComputeShader *m_dxComputeShader;
	RHIDevice* m_device;
	// Additional information from shader reflection I also usually store on this class.
};
