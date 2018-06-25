#pragma once

class RHIDevice;
class RHIInstance;
class RHIDeviceContext;
class RHIOutput;
class BlendState;
class ConstantBuffer;
class Font;
class RasterState;
class Sampler;
class ShaderProgram;
class Texture2D;
class VertexBuffer;
class IndexBuffer;
class StructuredBuffer;
class ComputeShader;
class ComputeJob;

//------------------------------------------------------------------------
// Determines how a resource is meant to be used.
enum BufferUsage : unsigned int
{
	BUFFERUSAGE_GPU,     // GPU Read/Write, CPU Full Writes        (RenderTargets?)
	BUFFERUSAGE_STATIC,  // GPU Read, Written only at creation     (TEXTURES, STATIC MESHES)
	BUFFERUSAGE_DYNAMIC, // GPU Read, CPU Write (Partial or Full)  (DYNAMIC MESHES/DYNAMIC TEXTURES)
	BUFFERUSAGE_STAGING, // GPU Read/Write, CPU Read/Write         Slowest - image composition/manipulation
};

//------------------------------------------------------------------------
enum PrimitiveType : unsigned int
{
	PRIMITIVE_NONE,
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_QUADS
};

struct vec3
{
	float x;
	float y;
	float z;

	vec3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f) {}

	vec3(float v)
		: x(v)
		, y(v)
		, z(v) {}

	vec3(float _x, float _y, float _z)
		: x(_x)
		, y(_y)
		, z(_z) {}
};

struct vec2
{
	float x;
	float y;

	vec2()
		: x(0.0f)
		, y(0.0f)
	{}

	vec2(float v)
		: x(v)
		, y(v)
	{}

	vec2(float _x, float _y)
		: x(_x)
		, y(_y)
	{}

};

struct vec4
{
	float x;
	float y;
	float z;
	float w;

	vec4()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{}

	vec4(float v)
		: x(v)
		, y(v)
		, z(v) 
		, w(v)
	{}

	vec4(float _x, float _y, float _z, float _w)
		: x(_x)
		, y(_y)
		, z(_z) 
		, w(_w)
	{}
};

struct vertex_t
{
public:
	vec3 position;
	vec2 uv;
	vec4 color;

public:
	vertex_t() {}
	vertex_t(vec3 const &pos, vec2 const &tex = vec2(0.0f), vec4 const &col = vec4(1.0f))
		: position(pos)
		, uv(tex)
		, color(col)
	{}
};

enum StructureBufferSlot : unsigned int
{

};

enum ConstantBufferSlot : unsigned int
{

};