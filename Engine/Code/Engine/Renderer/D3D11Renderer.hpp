#pragma once

#include <vector>
#include "Engine\Core\RGBA.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Math\Matrix4.hpp"
#include <map>
#include "Engine\RHI\DepthStencilState.hpp"
#include <stack>
#include "Mesh.hpp"
#include "MeshBuilder.hpp"
#include "Engine\RHI\Font.hpp"
#include "Skeleton.hpp"
#include "Engine\Tools\VoxelModel.hpp"
#include "Engine\RHI\Material.hpp"
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\Core\Window.hpp"
#include "Engine\RHI\Sampler.hpp"
#include "..\RHI\RasterState.hpp"
#include "Light.hpp"
#include "CameraZXY.hpp"

struct BlendStateInfo
{
	bool enabled;
	BlendFactor src;
	BlendFactor dest;
};

struct MatrixConstantBuffer
{
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;
	Vector4 eyePosition;
};

struct LightBuffer
{
	Vector4 AMBIENT_FACTOR; // (r,g,b,intensity)

	PointLightAttributes POINT_LIGHTS[8];


	float SPEC_FACTOR;
	float SPEC_POWER; 
	float LIGHT_PADDING[2];

};

class D3D11Renderer
{
public:
	D3D11Renderer();
	~D3D11Renderer();

	void Setup(unsigned int width, unsigned int height);

	Window* GetWindow();

	// Cleanup - could just be the destructor.
	void Destroy();

	bool IsClosed();

	void SetRenderTarget(Texture2D *colorTarget, Texture2D *depthTarget = nullptr);

	VertexBuffer * CreateVertexBuffer(Vertex3* vertices, unsigned int numOfVertices, PrimitiveType topology = PRIMITIVE_TRIANGLES, bool addBack = false, bool overWriteNT = true);
	VertexBuffer * CreateVertexBuffer(std::vector<Vertex3>& vertices, PrimitiveType topology = PRIMITIVE_TRIANGLES, bool addBack = false, bool overWriteNT = true);
	VertexBuffer * CreateTbnVertexBuffer(Vertex3* vertices, unsigned int numOfVertices, PrimitiveType topology = PRIMITIVE_TRIANGLES);
	IndexBuffer * CreateIndexBuffer(uint32_t* indices, unsigned int numOfIndices);
	IndexBuffer * CreateIndexBuffer(std::vector<uint32_t>& indices);

	void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
	void SetViewport();

	// Set the viewport using percentages of the size of the current render target.
	void SetViewportAsPercent(float x, float y, float w, float h);

	// Clears currently bound target
	void ClearColor(Rgba const &color);
	void ClearColor(unsigned int colorCode);

	// Clears specified target
	void ClearTargetColor(Texture2D *target, Rgba const &color);

	// Calls Outputs Present
	void Present() const;

	bool SetShader(ShaderProgram *shader);
	bool SetShader(const std::string& shaderName);
	ShaderProgram* CreateOrGetShader(const std::string& shaderName);
	ComputeShader* CreateOrGetComputeShader(const std::string& shaderName);

	void DrawVbo(VertexBuffer *vbo);
	void DrawVbo(PrimitiveType topology, VertexBuffer *vbo);
	void DrawVertices(std::vector<Vertex3> vertices, PrimitiveType primitiveType = PRIMITIVE_TRIANGLES, bool addBack = false, bool overrideNT = false);
	//void DrawVbo(PrimitiveType topology, std::vector<VertexBuffer>& vbo);
	void DrawIndexedVbo(PrimitiveType topology, VertexBuffer* vertexBuffer, IndexBuffer* indexedBuffer);
	//void DrawIndexedVbo(PrimitiveType topology, const std::vector<IndexBuffer*>& indexedBuffers);
	void DrawText2D(Vector2 pos, float scale, Rgba color, Font* font, std::string string, TextAlignHorizontal align = TEXTALIGN_HORIZONTAL_LEFT, int cursorIndex = -1);

	void SetRasterState(CullMode cullMode = CULLMODE_BACK, FillMode = FILLMODE_SOLID);

	void SetSampler(Sampler* sampler, unsigned int textureIndex = 0);
	void SetSampler(FilterMode minFilter, FilterMode magFilter);
	void SetComputeSampler(Sampler* sampler, unsigned int textureIndex = 0);
	void SetComputeSampler(FilterMode minFilter, FilterMode magFilter);

	void SetTexture(Texture2D* texture, unsigned int textureIndex = 0);
	void SetNormalTexture(Texture2D* texNormal);
	Texture2D* CreateOrGetTexture(const std::string& filePath);

	void SetConstantBuffer(unsigned idx, ConstantBuffer * buffer);

	void EnableBlend(BlendFactor src, BlendFactor dest);
	void DisableBlend();

	void EnableDepthTest(bool enabled = true);
	void EnableDepthWrite(bool enabled = true);

	void SetOrthoProjection(IntVector2 size);
	void SetOrthoProjection(float width, float height);
	void SetOrthoProjection(float nearX, float nearY, float farX, float farY);
	
	void ClearDepth(float depth = 1.0f, uint8_t stencil = 0);
	void SetViewMatrix(const Matrix4& view);
	void SetModelMatrix(const Matrix4& matrix);

	void SetAmbientLight(const Rgba& color, float intensity = 1.f);
	void EnablePointLight(uint lightIndex, const Vector3& position, const Rgba& color, float intensity, const Vector4& attenuation, const Vector4& specAttenuation);
	void EnablePointLight(uint lightIndex, const Light& light);
	void DisablePointLight(uint lightIndex);
	void DisableAllPointLight();

	void SetDiffuseTexture(Texture2D* texDiffuse);
	void SetSpecularTexture(Texture2D* texSpec);
	void SetEyePosition(const Vector3& eyePosition);
	void SetSpecularConstants(float specPower, float specFactor);

	void AddDebugLines(const Vector3& start, const Vector3& end, const Rgba& color = Rgba::WHITE);
	void DrawDebugLines();
	void ClearDebugLines();

	void PopMatrix();
	void PushMatrix();
	void PushMatrix(const Matrix4& matrix);
	void Translate(const Vector3& translate);
	void Translate3D(const Vector3& translate);
	void Translate3D(float x, float y, float z);
	void Scale(float uniformScale);
	void Scale(const Vector3& scale);
	void RotateX(float rotationX);
	void RotateY(float rotationY);
	void RotateZ(float rotationZ);
	void Rotate(float rotationDegrees, const Vector3& axis);
	void Rotate3D(float rotationDegrees, const Vector3& axis);
	void MultMatrix(const Matrix4& mat);

	void GenerateVBOBuffer(int numOfVbos, unsigned int* out_vboIndex);
	void Scale3D(float scale);

	void SetRaster(bool frontCounterClockwise = true);

	Mesh* BuildMesh(MeshBuilder& mb);
	void BuildMeshes(MeshBuilder& mb, std::vector<Mesh*>& meshes);
	void BuildMesh(Mesh& mesh);
	void BuildMeshes(std::vector<Mesh*>& meshes);
	Mesh* CreateMesh(const std::string & filePath);
	Mesh* GetMesh(const std::string & filePath);
	Mesh* CreateOrGetMesh(const std::string & filePath);
	void DrawMeshes(const std::vector<Mesh*> &meshes);
	void DrawMesh(Mesh* mesh);

	void DrawVoxelModel(const VoxelModel* model);
	VoxelModel* CreateOrGetVoxelModel(const std::string& filePath);

	void SetMaterial(const Material* material);
	void SetMaterial(const std::string& materialName);

	void DrawParticleSystem(const Vector3& cameraPosition = Vector3::ZERO);

	Texture2D* CaptureScreen();
/*
	void BuildSkeletonMesh(Skeleton* skeleton);
	Skeleton* CreateSkeletonMesh(const std::string & filePath);
	Skeleton* GetSkeletonMesh(const std::string & filePath);
	Skeleton* CreateOrGetSkeletonMesh(const std::string & filePath);
	void BuildSkeletonJointMesh(SkeletonJoint* joint);
	void DrawSkeleton(Skeleton* skeleton);
	void DrawSkeletonJoint(SkeletonJoint* joint);*/

	void DrawSkeletonInstance(SkeletonInstance* m_skeletonInstance);
	void SetStructuredBuffer(unsigned int bindSlot, StructuredBuffer* buffer);
	void DrawSkeleton(Skeleton* skeleton);
	void SetCamera(const CameraZXY& camera);

	void SetupConsole();
	void DrawConsole();

	void DrawProfiler();

	void SetComputeShader(ComputeShader * shader);
	void DispatchComputeJob(ComputeJob* computeJob);
	void SetUnorderedAccessViews(unsigned int slotIndex, Texture2D* texture);
	void SetComputeConstantBuffer(unsigned int slotIndex, ConstantBuffer* constantBuffer);
public:
	// RHI needed stuff
	RHIDeviceContext *m_rhiContext;
	RHIDevice *m_rhiDevice;
	RHIOutput *m_rhiOutput;

	Texture2D *m_currentTarget;
	RasterState *m_rasterState;

	LightBuffer m_lightBuffer;
	ConstantBuffer* m_lightConstantBuffer;

	BlendStateInfo m_blendState;
	BlendState *m_currentBlendState;

	MatrixConstantBuffer m_matrix;
	ConstantBuffer *m_matrixConstantBuffer;

	DepthStencilDesc m_depthStencilDesc;
	DepthStencilState *m_depthStencilState;

	ShaderProgram* m_defaultShader;
	Material* m_defaultMaterial;

	std::map<std::string, ShaderProgram*> m_shaders;
	std::map<std::string, ComputeShader*> m_computeShaders;
	std::map<std::string, Texture2D*> m_textures;
	std::map<std::string, Mesh*> m_meshes;
	std::map<std::string, Skeleton*> m_skeletons;
	std::map<std::string, VoxelModel*> m_voxelModels;
	std::map<std::string, Material*> m_materials;
	Sampler* m_sampler;
	Sampler* m_computeSampler;

	Texture2D *m_defaultDepthStencil;
	Texture2D *m_currentDepthStencil;

	std::stack<Matrix4> m_modelMatrices;

	std::vector<Vertex3> m_debugLines;

	// Whatever else you need to make this work.
	
	IntVector2 GetWindowSize() const;
	void SetPerspectiveProjection(float fieldOfView, float ratio, float nearZ, float farZ);

	void BuildTbn(std::vector<Vertex3>& vertices);
	VertexBuffer* BuildTbnVbo(const std::vector<Vertex3>& vertices);

private:
	void SetProjectionMatrix(const Matrix4& projection);
	Texture2D* GetTexture(const std::string& filePath);
	Texture2D* CreateTexture(const std::string& filePath);
	
	ShaderProgram* GetShader(const std::string& shaderName);
	ShaderProgram* CreateShader(const std::string& shaderName);
	
	ComputeShader* GetComputeShader(const std::string& shaderName);
	ComputeShader* CreateComputeShader(const std::string& shaderName);

};

inline D3D11Renderer::D3D11Renderer()
	:m_rhiContext(nullptr)
	,m_rhiDevice(nullptr)
	,m_rhiOutput(nullptr)
	,m_currentTarget(nullptr)
	,m_rasterState(nullptr)
	,m_currentBlendState(nullptr)
	,m_matrixConstantBuffer(nullptr)
	,m_defaultShader(nullptr)
	,m_sampler(nullptr)
{

}
