#include "D3D11Renderer.hpp"
#include <d3d11.h>

#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\RHI\Font.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Core\RGBAf.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "MeshBuilder.hpp"
#include "Engine\Tools\fbx.hpp"
#include "Engine\RHI\IndexBuffer.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Core\Config.hpp"
#include "Engine\Tools\Memory.hpp"
#include "Engine\Config\BuildConfig.hpp"
#include "Engine\Core\Profiler.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "ParticleSystem.hpp"
#include "Engine\RHI\RHIInstance.hpp"
#include "Engine\RHI\RasterState.hpp"
#include "Engine\RHI\ConstantBuffer.hpp"
#include "Engine\RHI\RHIOutput.hpp"
#include "CameraZXY.hpp"


D3D11Renderer::~D3D11Renderer()
{
	
}

void D3D11Renderer::Setup(unsigned int width, unsigned int height)
{

	RHIInstance *ri = RHIInstance::GetInstance();
	ri->CreateOutput(m_rhiDevice, m_rhiContext, m_rhiOutput, width, height);
	//m_rhiDevice->CreateShaderProgramFromHlslFile("Data/hlsl/nop_color.hlsl");
	
	m_rasterState = new RasterState(m_rhiDevice);
	m_rhiContext->SetRasterState(m_rasterState);

	m_defaultShader = CreateOrGetShader("NoEffect");
	m_textures[""] = new Texture2D(m_rhiDevice,Image());
	m_textures["normal_default"] = new Texture2D(m_rhiDevice, Image(Rgba(128,128,255)));

	m_defaultMaterial = new Material();

	m_matrixConstantBuffer = new ConstantBuffer(m_rhiDevice, &m_matrix, sizeof(m_matrix));
	SetConstantBuffer(0, m_matrixConstantBuffer);

	m_defaultDepthStencil = new Texture2D(m_rhiDevice, width, height, IMAGEFORMAT_D24S8);
	m_currentDepthStencil = nullptr;

	m_sampler = nullptr;
	m_computeSampler = nullptr;

	m_rhiOutput->m_shader = this;

	SetRenderTarget(nullptr, nullptr);

	m_depthStencilDesc.depthTestEnabled = true;
	m_depthStencilDesc.depthWritingEnabled = true;
	m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
	m_rhiContext->SetDepthStencilState(m_depthStencilState);

	m_lightBuffer.AMBIENT_FACTOR = Vector4::ONE;
	m_lightBuffer.SPEC_FACTOR = 1.f;
	m_lightBuffer.SPEC_POWER = 1.f;

	m_lightConstantBuffer = new ConstantBuffer(m_rhiDevice, &m_lightBuffer, sizeof(m_lightBuffer));
	SetConstantBuffer(2, m_lightConstantBuffer);

	m_matrix.eyePosition = Vector4(Vector3::ZERO, 1.f);

	m_modelMatrices.push(Matrix4::CreateIdentity());
}

Window* D3D11Renderer::GetWindow()
{
	return m_rhiOutput->m_window;
}

void D3D11Renderer::Destroy()
{
	if (!m_shaders.empty())
		for (auto shader : m_shaders)
		{
			SAFE_DELETE(shader.second);
		}
	if (!m_textures.empty())
		for (auto texture : m_textures)
		{
			SAFE_DELETE(texture.second);
		}
	SAFE_DELETE(m_rhiDevice);
	SAFE_DELETE(m_rhiOutput);
	if(m_currentDepthStencil != m_defaultDepthStencil)
		SAFE_DELETE(m_currentDepthStencil);
	SAFE_DELETE(m_defaultDepthStencil);
	
}

bool D3D11Renderer::IsClosed()
{
	return m_rhiOutput->m_window->IsClosed();
}

void D3D11Renderer::SetRenderTarget(Texture2D *colorTarget, Texture2D *depthTarget /*= nullptr*/)
{
	if (colorTarget != nullptr) {
		if (colorTarget->IsRenderTarget()) {
			m_currentTarget = colorTarget;
		} // else, WTH?
	}
	else {
		m_currentTarget = m_rhiOutput->GetRenderTarget();
	}

	if (depthTarget == nullptr) {
		depthTarget = m_defaultDepthStencil;
	}

	m_currentDepthStencil = depthTarget;
	m_rhiContext->SetColorTarget(m_currentTarget, m_currentDepthStencil);
}

VertexBuffer * D3D11Renderer::CreateVertexBuffer(Vertex3* vertex, unsigned int numOfVertices, PrimitiveType topology /*= PRIMITIVE_TRIANGLES*/, bool addBack /*= false*/, bool overWriteNT /*= true*/)
{
	switch (topology)
	{
	case PRIMITIVE_QUADS:
	{
		PROFILE_SCOPE("Triangulating quad vertices");
		std::vector<Vertex3> vertices;
		unsigned int bufferSize = (unsigned int)(numOfVertices * 1.5f * (addBack ? 2.f : 1.f));
		vertices.reserve(bufferSize);

		for (unsigned int offset = 0; offset < numOfVertices; offset += 4)
		{
			/*vertices.push_back(vertex[offset]);
			vertices.push_back(vertex[offset + 1]);
			vertices.push_back(vertex[offset + 2]);
			vertices.push_back(vertex[offset]);
			vertices.push_back(vertex[offset + 2]);
			vertices.push_back(vertex[offset + 3]);*/
			vertices.emplace_back(vertex[offset]);
			vertices.emplace_back(vertex[offset + 1]);
			vertices.emplace_back(vertex[offset + 2]);
			vertices.emplace_back(vertex[offset]);
			vertices.emplace_back(vertex[offset + 2]);
			vertices.emplace_back(vertex[offset + 3]);

			if (addBack)
			{
				/*vertices.push_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.push_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
				vertices.push_back(Vertex3(vertex[offset + 1].position, vertex[offset + 1].color, vertex[offset + 1].texCoords, -vertex[offset + 1].normal, -vertex[offset +1 ].tangent));
				vertices.push_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.push_back(Vertex3(vertex[offset + 3].position, vertex[offset + 3].color, vertex[offset + 3].texCoords, -vertex[offset + 3].normal, -vertex[offset + 3].tangent));
				vertices.push_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
				*/
				vertices.emplace_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.emplace_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
				vertices.emplace_back(Vertex3(vertex[offset + 1].position, vertex[offset + 1].color, vertex[offset + 1].texCoords, -vertex[offset + 1].normal, -vertex[offset + 1].tangent));
				vertices.emplace_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.emplace_back(Vertex3(vertex[offset + 3].position, vertex[offset + 3].color, vertex[offset + 3].texCoords, -vertex[offset + 3].normal, -vertex[offset + 3].tangent));
				vertices.emplace_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
			}
		}

		if(overWriteNT)
			BuildTbn(vertices);

		return m_rhiDevice->CreateVertexBuffer(vertices.data(), vertices.size());
		break;
	}
	case PRIMITIVE_TRIANGLES:
		if (overWriteNT)
		{
			std::vector<Vertex3> vertices(vertex,vertex+numOfVertices);
			BuildTbn(vertices);
			return m_rhiDevice->CreateVertexBuffer(vertex, numOfVertices);
		}
	default:
		break;
	}

	
	return m_rhiDevice->CreateVertexBuffer(vertex, numOfVertices);
}

VertexBuffer * D3D11Renderer::CreateVertexBuffer(std::vector<Vertex3>& vertices, PrimitiveType topology, bool addBack, bool overWriteNT)
{
	return CreateVertexBuffer(vertices.data(), vertices.size(), topology, addBack, overWriteNT);
	/*switch (topology)
	{
	case PRIMITIVE_QUADS:
	{
		PROFILE_SCOPE("Triangulating quad vertices");
		unsigned int quadSize = vertices.size() / 4;
		unsigned int bufferSize = (unsigned int)(vertices.size() * 1.5f * (addBack ? 2.f : 1.f));
		vertices.reserve(bufferSize);

		for (uint quadIndex = 0; quadIndex < quadSize; quadIndex++)
		{
			Vertex3 q0 = vertices[quadIndex * 6 + 0];
			Vertex3 q2 = vertices[quadIndex * 6 + 2];
			vertices.insert(vertices.begin() + quadIndex * 6 + 3, q0);
			vertices.insert(vertices.begin() + quadIndex * 6 + 4, q2);
		}

		for (unsigned int offset = 0; offset < numOfVertices; offset += 4)
		{
			vertices.push_back(vertex[offset]);
			vertices.push_back(vertex[offset + 1]);
			vertices.push_back(vertex[offset + 2]);
			vertices.push_back(vertex[offset]);
			vertices.push_back(vertex[offset + 2]);
			vertices.push_back(vertex[offset + 3]);

			if (addBack)
			{
				vertices.push_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.push_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
				vertices.push_back(Vertex3(vertex[offset + 1].position, vertex[offset + 1].color, vertex[offset + 1].texCoords, -vertex[offset + 1].normal, -vertex[offset + 1].tangent));
				vertices.push_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.push_back(Vertex3(vertex[offset + 3].position, vertex[offset + 3].color, vertex[offset + 3].texCoords, -vertex[offset + 3].normal, -vertex[offset + 3].tangent));
				vertices.push_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
			}
		}

		if (overWriteNT)
			BuildTbn(vertices);

		return m_rhiDevice->CreateVertexBuffer(vertices.data(), vertices.size());
		break;
	}
	default:
		break;
	}


	return m_rhiDevice->CreateVertexBuffer(vertices.data(), vertices.size());*/
}

VertexBuffer * D3D11Renderer::CreateTbnVertexBuffer(Vertex3* vertex, unsigned int numOfVertices, PrimitiveType topology /*= PRIMITIVE_TRIANGLES*/)
{
	switch (topology)
	{
	case PRIMITIVE_QUADS:
	{
		std::vector<Vertex3> vertices;
		unsigned int bufferSize = (unsigned int)(numOfVertices * 1.5);
		vertices.reserve(bufferSize);

		for (unsigned int offset = 0; offset < numOfVertices; offset += 4)
		{

			//LB -> RB -> RT -> LT
			Vector3 right = Interpolate<Vector3>(vertex[offset + 1].position - vertex[offset].position, vertex[offset + 2].position - vertex[offset + 3].position, 0.5f);
			Vector2 rightUvDelta = Interpolate<Vector2>(vertex[offset + 1].texCoords - vertex[offset].texCoords, vertex[offset + 2].texCoords - vertex[offset + 3].texCoords, 0.5f);
			if (rightUvDelta.x)
				right *= rightUvDelta.x;

			Vector3 up = Interpolate<Vector3>(vertex[offset + 3].position - vertex[offset].position, vertex[offset + 2].position - vertex[offset + 1].position, 0.5f);
			Vector2 upUvDelta = Interpolate<Vector2>(vertex[offset + 3].texCoords - vertex[offset].texCoords, vertex[offset + 2].texCoords - vertex[offset + 1].texCoords, 0.5f);
			if (upUvDelta.y)
				up *= -upUvDelta.y;


			Vector3 normal = -CrossProduct(right, up);//CrossProduct(up, right);
			Vector3 tangent = right;

			Vector3 center = (vertex[offset].position + vertex[offset + 1].position + vertex[offset + 2].position + vertex[offset + 3].position) / 4.f;

			right.Normalize();
			up.Normalize();
			normal.Normalize();

			vertices.push_back(Vertex3(center, Rgba::RED));
			vertices.push_back(Vertex3(center + right, Rgba::RED));
			vertices.push_back(Vertex3(center, Rgba::GREEN));
			vertices.push_back(Vertex3(center + up, Rgba::GREEN));
			vertices.push_back(Vertex3(center, Rgba::BLUE));
			vertices.push_back(Vertex3(center + normal, Rgba::BLUE));

		}

		return m_rhiDevice->CreateVertexBuffer(vertices.data(), vertices.size());
		break;
	}
	default:
		break;
	}


	return m_rhiDevice->CreateVertexBuffer(vertex, numOfVertices);
}

IndexBuffer * D3D11Renderer::CreateIndexBuffer(uint32_t * indices, unsigned int numOfIndices)
{
	return m_rhiDevice->CreateIndexBuffer(indices,numOfIndices);
}

IndexBuffer * D3D11Renderer::CreateIndexBuffer(std::vector<uint32_t>& indices)
{
	return CreateIndexBuffer(indices.data(),indices.size());
}

void D3D11Renderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_rhiContext->SetViewports(x,y,width,height);
}

void D3D11Renderer::SetViewport()
{
	IntVector2 windowSize = m_rhiOutput->m_window->GetWindowSize();
	m_rhiContext->SetViewports(0, 0, windowSize.x, windowSize.y);
}

void D3D11Renderer::SetViewportAsPercent(float x, float y, float w, float h)
{
	UNUSED(x);
	UNUSED(y);
	UNUSED(w);
	UNUSED(h);
}

void D3D11Renderer::ClearColor(Rgba const & color)
{
	m_rhiContext->ClearColorTarget(m_currentTarget, color);
}

void D3D11Renderer::ClearColor(unsigned int colorCode)
{
	ClearColor(Rgba(colorCode));
}

void D3D11Renderer::ClearTargetColor(Texture2D * target, Rgba const & color)
{
	m_rhiContext->ClearColorTarget(target, color);
}

void D3D11Renderer::Present() const
{
	m_rhiOutput->Present();
}

bool D3D11Renderer::SetShader(ShaderProgram * shader)
{
	bool result = true;
	if (shader == nullptr)
	{
		shader = m_defaultShader;
		result = false;
	}
	m_rhiContext->SetShader(shader);
	return result;
}

bool D3D11Renderer::SetShader(const std::string& shaderName)
{
	ShaderProgram* shader = CreateOrGetShader(shaderName);
	return SetShader(shader);
}

ShaderProgram* D3D11Renderer::CreateOrGetShader(const std::string& shaderName)
{
	if (shaderName.empty())
		return m_defaultShader;
	std::map<std::string, ShaderProgram*>::iterator found = m_shaders.find(shaderName);
	if (found != m_shaders.end())
		return GetShader(shaderName);
	else
	{
		ShaderProgram* shader = CreateShader(shaderName);
		return shader == nullptr ? m_defaultShader : shader;
	}
}

ComputeShader* D3D11Renderer::CreateOrGetComputeShader(const std::string& shaderName)
{
	auto found = m_computeShaders.find(shaderName);
	if (found != m_computeShaders.end())
		return GetComputeShader(shaderName);
	return CreateComputeShader(shaderName);
}

ShaderProgram* D3D11Renderer::GetShader(const std::string& shaderName)
{
	std::map<std::string, ShaderProgram*>::iterator found = m_shaders.find(shaderName);
	if (found != m_shaders.end())
		return found->second;
	else
		return nullptr;
}

ShaderProgram * D3D11Renderer::CreateShader(const std::string& shaderName)
{
	ShaderProgram* shader = m_rhiDevice->CreateShaderProgramFromHlslFile("Data/hlsl/" + shaderName + ".hlsl");
	//ASSERT_OR_DIE(shader, "Shader compile error!");
	if (shader)
		m_shaders[shaderName] = shader;
	return shader;
}

ComputeShader* D3D11Renderer::GetComputeShader(const std::string& shaderName)
{
	auto found = m_computeShaders.find(shaderName);
	if (found != m_computeShaders.end())
		return found->second;
	return nullptr;
}

ComputeShader* D3D11Renderer::CreateComputeShader(const std::string& shaderName)
{
	ComputeShader* cs = m_rhiDevice->CreateComputeShaderFromHlslFile("Data/hlsl/" + shaderName + ".hlsl");
	if (cs)
		m_computeShaders[shaderName] = cs;
	return cs;
}

void D3D11Renderer::DrawSkeletonInstance(SkeletonInstance* m_skeletonInstance)
{
	std::vector<Vertex3> vertices = MeshBuilder::SkeletonIns(m_skeletonInstance);

	VertexBuffer* vbo = CreateVertexBuffer(vertices);

	DrawVbo(PRIMITIVE_LINES,vbo);

	SAFE_DELETE(vbo);
}

void D3D11Renderer::SetStructuredBuffer(unsigned int bindSlot, StructuredBuffer* buffer)
{
	m_rhiContext->SetStructuredBuffer(bindSlot, buffer);
}

void D3D11Renderer::DrawSkeleton(Skeleton* skeleton)
{

	std::vector<Vertex3> vertices = MeshBuilder::SkeletonLines(skeleton);

	VertexBuffer* vbo = CreateVertexBuffer(vertices);

	DrawVbo(PRIMITIVE_LINES, vbo);

	SAFE_DELETE(vbo);
}

void D3D11Renderer::SetCamera(const CameraZXY& camera)
{
	IntVector2 windowSize = GetWindowSize();
	float aspectRatio = (float)windowSize.x / (float)windowSize.y;
	SetEyePosition(camera.m_position);
	SetViewMatrix(camera.GetViewMatrix());
	SetPerspectiveProjection(camera.m_fov, aspectRatio, camera.m_near, camera.m_far);
}

void D3D11Renderer::BuildTbn(std::vector<Vertex3>& vertices)
{
	PROFILE_FUNCTION;
	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		// Shortcuts for vertices
		Vector3 & v0 = vertices[i + 0].position;
		Vector3 & v1 = vertices[i + 1].position;
		Vector3 & v2 = vertices[i + 2].position;

		// Shortcuts for UVs
		Vector2 & uv0 = vertices[i + 0].texCoords;
		Vector2 & uv1 = vertices[i + 1].texCoords;
		Vector2 & uv2 = vertices[i + 2].texCoords;

		// Edges of the triangle : postion delta
		Vector3 deltaPos1 = v1 - v0;
		Vector3 deltaPos2 = v2 - v0;

		// UV delta
		Vector2 deltaUV1 = uv1 - uv0;
		Vector2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		Vector3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		Vector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
		Vector3 normal = CrossProduct(v0 - v1, v2 - v1);//CrossProduct(tangent, bitangent);

		vertices[i + 0].tangent = tangent;
		vertices[i + 1].tangent = tangent;
		vertices[i + 2].tangent = tangent;

		vertices[i + 0].normal = normal;
		vertices[i + 1].normal = normal;
		vertices[i + 2].normal = normal;
	}
}

void D3D11Renderer::DrawVbo(PrimitiveType topology, VertexBuffer * vbo)
{
	
	m_rhiContext->SetPrimitiveTopology(topology);

	m_rhiContext->SetVertexBuffer(vbo);

	m_rhiContext->Draw(vbo->m_numOfVertices, 0);
	
}

void D3D11Renderer::DrawVbo(VertexBuffer *vbo)
{
	DrawVbo(PRIMITIVE_TRIANGLES, vbo);
}

void D3D11Renderer::DrawVertices(std::vector<Vertex3> vertices, PrimitiveType primitiveType /*= PRIMITIVE_TRIANGLES*/, bool addBack /*= false*/, bool overrideNT /*= false*/)
{
	VertexBuffer* vbo = CreateVertexBuffer(vertices, primitiveType, addBack, overrideNT);
	DrawVbo(vbo);
	delete vbo;
}

void D3D11Renderer::DrawIndexedVbo(PrimitiveType topology, VertexBuffer * vertexBuffer, IndexBuffer * indexedBuffer)
{
	m_rhiContext->SetPrimitiveTopology(topology);
	m_rhiContext->SetVertexBuffer(vertexBuffer);
	m_rhiContext->SetIndexBuffer(indexedBuffer);
	m_rhiContext->DrawIndexed(indexedBuffer->m_numOfIndices, 0, 0);
}

void D3D11Renderer::DrawText2D(Vector2 pos, float scale, Rgba color, Font* font, std::string string, TextAlignHorizontal align /*= TEXTALIGN_LEFT*/, int cursorIndex /*= -1*/)
{

	UNUSED(cursorIndex);
	UNUSED(scale);
	UNUSED(align);
	Vector2 currentPos = pos;
	Vector2 lineStartPos = pos;

	std::vector<Vertex3> vertices = MeshBuilder::Text2D(pos,scale,color,font,string,align,TEXTALIGN_VERTICAL_BOTTOM);

	/*for (size_t stringIndex = 0; stringIndex < string.size(); stringIndex++)
	{
		char printChar = string[stringIndex];

		if (printChar == '\n')
		{
			lineStartPos.y -= font->m_lineHeight * scale;
			currentPos = lineStartPos;
			continue;
		}

		std::map<int, FontTag>::iterator tagFound = font->m_tags.find(printChar);

		ASSERT_OR_DIE(tagFound != font->m_tags.end(), "undefined font tags of char " + printChar);
// 		if (tag == font->m_tags.end())
// 			continue;

		FontTag tag = tagFound->second;


		Vector2 charOffset(tag.xoffset, -tag.yoffset - tag.height + font->m_base);

		if (stringIndex > 0)
		{
			char prevChar = string[stringIndex];
			std::map<std::pair<int, int>, int>::iterator kerningFound = font->m_kernings.find(std::pair<int, int>(prevChar,printChar));
			if (kerningFound != font->m_kernings.end())
				charOffset.x += kerningFound->second;
		}

		Vector2 drawPos = currentPos + charOffset * scale;
		Vector2 charSize(tag.width, tag.height);

		AABB2 quad(drawPos, drawPos + charSize * scale);

		AABB2 uv((float)tag.x, (float)tag.y, (float)(tag.x + tag.width), (float)(tag.y + tag.height));
		uv.mins.x /= font->m_scaleW;
		uv.maxs.x /= font->m_scaleW;
		uv.mins.y /= font->m_scaleH;
		uv.maxs.y /= font->m_scaleH;

		float fColorR;
		float fColorG;
		float fColorB;
		float fColorA;

		color.GetAsFloats(fColorR, fColorG, fColorB, fColorA);

		vec4 drawColor(fColorR, fColorG, fColorB, fColorA);

		Vertex3 newVertices[4] = {
			Vertex3(Vector3(quad.mins.x,quad.mins.y,0.f), color, Vector2(uv.mins.x,uv.maxs.y)),
			Vertex3(Vector3(quad.maxs.x,quad.mins.y,0.f), color, Vector2(uv.maxs.x,uv.maxs.y)),
			Vertex3(Vector3(quad.maxs.x,quad.maxs.y,0.f), color, Vector2(uv.maxs.x,uv.mins.y)),
			Vertex3(Vector3(quad.mins.x,quad.maxs.y,0.f), color, Vector2(uv.mins.x,uv.mins.y)),
		};

		vertices.insert(vertices.end(),&newVertices[0], &newVertices[4]);

		currentPos.x += tag.xadvance * scale;
	}*/

	SetTexture(font->m_texture);
	VertexBuffer* vbo = CreateVertexBuffer(vertices, PRIMITIVE_TRIANGLES, false, false);
	//VertexBuffer* vbo = CreateVertexBuffer(vertices.data(), vertices.size(), PRIMITIVE_QUADS);
	DrawVbo(PRIMITIVE_TRIANGLES, vbo);
	SAFE_DELETE(vbo);
}

void D3D11Renderer::SetRasterState(CullMode cullMode /*= CULLMODE_BACK*/, FillMode fillMode /*= FILLMODE_SOLID*/)
{
	if (!m_rasterState)
	{
		m_rasterState = new RasterState(m_rhiDevice, cullMode, fillMode);
		m_rhiContext->SetRasterState(m_rasterState);
		return;
	}
	if (m_rasterState->m_cullMode == cullMode && m_rasterState->m_fillMode == fillMode)
		return;
	SAFE_DELETE(m_rasterState);
	m_rasterState = new RasterState(m_rhiDevice, cullMode, fillMode);
	m_rhiContext->SetRasterState(m_rasterState);
}

void D3D11Renderer::SetSampler(Sampler * sampler, unsigned int textureIndex)
{
	m_rhiContext->SetSampler(sampler, textureIndex);
}

void D3D11Renderer::SetSampler(FilterMode minFilter, FilterMode magFilter)
{
	if (m_sampler)
		SAFE_DELETE(m_sampler);
	m_sampler = new Sampler(m_rhiDevice, minFilter, magFilter);
	SetSampler(m_sampler);
}

void D3D11Renderer::SetComputeSampler(FilterMode minFilter, FilterMode magFilter)
{
	if (m_computeSampler)
		SAFE_DELETE(m_computeSampler);
	m_computeSampler = new Sampler(m_rhiDevice, minFilter, magFilter);
	SetComputeSampler(m_computeSampler);
}

void D3D11Renderer::SetComputeSampler(Sampler* sampler, unsigned int textureIndex /*= 0*/)
{
	m_rhiContext->SetComputeSampler(sampler, textureIndex);
}


void D3D11Renderer::SetTexture(Texture2D * texture, unsigned int textureIndex)
{
	if (texture == nullptr)
		texture = m_textures[""];

	m_rhiContext->SetShaderResources(textureIndex, texture);

	m_rhiContext->m_dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
	m_rhiContext->m_dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
}

void D3D11Renderer::SetConstantBuffer(unsigned idx, ConstantBuffer * constantBuffer)
{
	m_rhiContext->SetConstantBuffer(idx, constantBuffer);
}

void D3D11Renderer::EnableBlend(BlendFactor src, BlendFactor dest)
{

	if (m_currentBlendState != nullptr) {
		if ((m_blendState.enabled == true)
			&& (m_blendState.src == src)
			&& (m_blendState.dest == dest))
		{
			return;
		}
	}

	SAFE_DELETE(m_currentBlendState);

	BlendState *blendState = new BlendState(m_rhiDevice, true, src, dest);
	m_rhiContext->SetBlendState(blendState);
	m_currentBlendState = blendState;

	m_blendState.enabled = true;
	m_blendState.src = src;
	m_blendState.dest = dest;

}

void D3D11Renderer::DisableBlend()
{
	if (!m_blendState.enabled) {
		return;
	}
	
	SAFE_DELETE(m_currentBlendState);
	BlendState *bs = new BlendState(m_rhiDevice, false);
	m_rhiContext->SetBlendState(bs);
	m_blendState.enabled = false;
	m_currentBlendState = bs;
}

void D3D11Renderer::EnableDepthTest(bool enabled /*= true*/)
{
	if (m_depthStencilDesc.depthTestEnabled != enabled) {
		m_depthStencilDesc.depthTestEnabled = enabled;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void D3D11Renderer::EnableDepthWrite(bool enabled /*= true*/)
{
	if (m_depthStencilDesc.depthWritingEnabled != enabled) {
		m_depthStencilDesc.depthWritingEnabled = enabled;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void D3D11Renderer::SetOrthoProjection(float width, float height)
{
	SetOrthoProjection(0.f, 0.f, width, height);
}

void D3D11Renderer::SetOrthoProjection(float nearX, float nearY, float farX, float farY)
{
	Matrix4 projection = Matrix4::MatrixMakeOrthoProjection(nearX, nearY, farX, farY);
	SetProjectionMatrix(projection);
	SetViewMatrix(Matrix4::CreateIdentity());
}

void D3D11Renderer::SetOrthoProjection(IntVector2 size)
{
	SetOrthoProjection(0.f, 0.f, (float)size.x, (float)size.y);
}

void D3D11Renderer::ClearDepth(float depth/* = 1.0f*/, uint8_t stencil/* = 0*/)
{
	m_rhiContext->ClearDepthTarget(m_currentDepthStencil, depth, stencil);
}

void D3D11Renderer::SetViewMatrix(const Matrix4& view)
{
	m_matrix.view = view;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void D3D11Renderer::SetModelMatrix(const Matrix4& matrix)
{
	m_matrix.model = matrix;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void D3D11Renderer::SetAmbientLight(const Rgba& color, float intensity /*= 1.f*/)
{
	RgbaF colorF(color);
	Vector4 colorFactor(colorF.red, colorF.green, colorF.blue, intensity);
	if (colorFactor == m_lightBuffer.AMBIENT_FACTOR)
		return;
	m_lightBuffer.AMBIENT_FACTOR = colorFactor;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void D3D11Renderer::EnablePointLight(uint lightIndex, const Vector3& position, const Rgba& color, float intensity, const Vector4& attenuation, const Vector4& specAttenuation)
{
	ASSERT_RECOVERABLE(lightIndex < 8, "D3D11Renderer::EnablePointLight() lightIndex excceeded.");
	RgbaF colorF(color);
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_POSITION = Vector4(position);
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_COLOR = Vector4(colorF.red, colorF.green, colorF.blue, intensity);
	m_lightBuffer.POINT_LIGHTS[lightIndex].ATTENUATION = attenuation;
	m_lightBuffer.POINT_LIGHTS[lightIndex].SPEC_ATTENUATION = specAttenuation;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void D3D11Renderer::EnablePointLight(uint lightIndex, const Light& light)
{
	ASSERT_RECOVERABLE(lightIndex < 8, "D3D11Renderer::EnablePointLight() lightIndex excceeded.");
	m_lightBuffer.POINT_LIGHTS[lightIndex] = light.m_attributes;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void D3D11Renderer::DisablePointLight(uint lightIndex)
{
	ASSERT_RECOVERABLE(lightIndex < 8, "D3D11Renderer::EnablePointLight() lightIndex excceeded.");
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_POSITION = Vector4::ZERO;
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_COLOR = Vector4::ZERO;
	m_lightBuffer.POINT_LIGHTS[lightIndex].ATTENUATION = Vector4::ZERO;
	m_lightBuffer.POINT_LIGHTS[lightIndex].SPEC_ATTENUATION = Vector4::ZERO;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void D3D11Renderer::DisableAllPointLight()
{
	for (int i = 0; i < 8; i++)
		m_lightBuffer.POINT_LIGHTS[i] = PointLightAttributes();
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}


void D3D11Renderer::PushMatrix()
{
	m_modelMatrices.push(m_modelMatrices.top());
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::PushMatrix(const Matrix4& matrix)
{
	m_modelMatrices.push(matrix);
	SetModelMatrix(matrix);
}

void D3D11Renderer::PopMatrix()
{
	if(m_modelMatrices.size()>1)
		m_modelMatrices.pop();
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::Translate(const Vector3& translate)
{
	m_modelMatrices.top().Translate(translate);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::Translate3D(const Vector3 & translate)
{
	Translate(translate);
}

void D3D11Renderer::Translate3D(float x, float y, float z)
{
	Translate(Vector3(x, y, z));
}

void D3D11Renderer::Scale(float uniformScale)
{
	Scale(Vector3(uniformScale, uniformScale, uniformScale));
}

void D3D11Renderer::MultMatrix(const Matrix4& mat)
{
	m_modelMatrices.top().ConcatenateTransform(mat);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::SetDiffuseTexture(Texture2D* texDiffuse)
{
	if (!texDiffuse)
		texDiffuse = m_textures[""];
	SetTexture(texDiffuse, 0);
}

void D3D11Renderer::SetNormalTexture(Texture2D* texNormal)
{
	if (!texNormal)
		texNormal = m_textures["normal_default"];
	SetTexture(texNormal, 1);
}

void D3D11Renderer::SetSpecularTexture(Texture2D* texSpec)
{
	if (!texSpec)
		texSpec = m_textures[""];
	SetTexture(texSpec, 2);
}

void D3D11Renderer::SetEyePosition(const Vector3& eyePosition)
{
	m_matrix.eyePosition = Vector4(eyePosition,1.f);
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void D3D11Renderer::SetSpecularConstants(float specPower, float specFactor)
{
	m_lightBuffer.SPEC_POWER = specPower;
	m_lightBuffer.SPEC_FACTOR = specFactor;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void D3D11Renderer::AddDebugLines(const Vector3 & start, const Vector3 & end, const Rgba & color)
{
	m_debugLines.push_back(Vertex3(start,color));
	m_debugLines.push_back(Vertex3(end, color));
}

void D3D11Renderer::DrawDebugLines()
{
	if (m_debugLines.empty())
		return;
	VertexBuffer* debugVbo = CreateVertexBuffer(m_debugLines.data(), m_debugLines.size(), PRIMITIVE_LINES);
	DrawVbo(PRIMITIVE_LINES, debugVbo);
	SAFE_DELETE(debugVbo);
}

void D3D11Renderer::ClearDebugLines()
{
	m_debugLines.clear();
}

void D3D11Renderer::Scale(const Vector3& scale)
{
	m_modelMatrices.top().Scale(scale);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::Rotate(float rotationDegrees, const Vector3& axis)
{
	m_modelMatrices.top().RotateDegreesAboutAxis(rotationDegrees, axis);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::Rotate3D(float rotationDegrees, const Vector3& axis)
{
	Rotate(rotationDegrees, axis);
}

void D3D11Renderer::Scale3D(float scale)
{
	Scale(Vector3(scale, scale, scale));
}

void D3D11Renderer::SetRaster(bool frontCounterClockwise /*= true*/)
{
	SAFE_DELETE(m_rasterState);
	m_rasterState = new RasterState(m_rhiDevice, CULLMODE_BACK, FILLMODE_SOLID, frontCounterClockwise);
	m_rhiContext->SetRasterState(m_rasterState);
}
#if defined(TOOLS_BUILD)
Mesh * D3D11Renderer::BuildMesh(MeshBuilder & mb)
{
	VertexBuffer* vertexBuffer = CreateVertexBuffer(mb.m_vertices);

	if (!mb.m_useIndex)
	{

		return new Mesh(mb.m_primitiveType, mb.m_name, mb.m_vertices,vertexBuffer,nullptr, false, std::vector<uint32_t>(), nullptr, mb.m_blendShape);
	}

	IndexBuffer* indexBuffer = CreateIndexBuffer(mb.m_indices);

	return new Mesh(mb.m_primitiveType, mb.m_name, mb.m_vertices, vertexBuffer, nullptr, mb.m_useIndex, mb.m_indices, indexBuffer, mb.m_blendShape);
}

void D3D11Renderer::BuildMesh(Mesh& mesh)
{
	mesh.m_vertexBuffer = CreateVertexBuffer(mesh.m_vertices);

	if(mesh.m_useIndex)
		mesh.m_indexBuffer = CreateIndexBuffer(mesh.m_indices);
}

void D3D11Renderer::BuildMeshes(MeshBuilder& mb, std::vector<Mesh*>& meshes)
{
	if(!mb.m_vertices.empty())
		meshes.push_back(BuildMesh(mb));
	for (auto child : mb.m_children)
		BuildMeshes(*child, meshes);
}

void D3D11Renderer::BuildMeshes(std::vector<Mesh*>& meshes)
{
	for (auto mesh : meshes)
		BuildMesh(*mesh);
}

Mesh * D3D11Renderer::CreateMesh(const std::string & filePath)
{
	MeshBuilder mb;
	FBXUtils::LoadMesh(filePath, mb);
	return BuildMesh(mb);
}

Mesh* D3D11Renderer::GetMesh(const std::string & filePath)
{
	return m_meshes[filePath];
}

Mesh * D3D11Renderer::CreateOrGetMesh(const std::string & filePath)
{
	std::map<std::string, Mesh*>::iterator found = m_meshes.find(filePath);
	if (found == m_meshes.end())
		return CreateMesh(filePath);
	else
		return GetMesh(filePath);
}

void D3D11Renderer::DrawMeshes(const std::vector<Mesh*> &meshes)
{
	for (auto mesh : meshes)
	{
		SetMaterial(mesh->m_material);
		DrawMesh(mesh);
	}
}

void D3D11Renderer::DrawMesh(Mesh* mesh)
{
	
	if (mesh->m_useIndex)
	{
		if (!mesh->m_vertexBuffer)
			mesh->m_vertexBuffer = CreateVertexBuffer(mesh->m_vertices);
		if (!mesh->m_indexBuffer)
			mesh->m_indexBuffer = CreateIndexBuffer(mesh->m_indices);
		DrawIndexedVbo(PRIMITIVE_TRIANGLES, mesh->m_vertexBuffer, mesh->m_indexBuffer);
	}
	else
	{
		if (!mesh->m_vertexBuffer)
			mesh->m_vertexBuffer = CreateVertexBuffer(mesh->m_vertices);
		DrawVbo(PRIMITIVE_TRIANGLES, mesh->m_vertexBuffer);
	}
}

/*
void D3D11Renderer::BuildSkeletonMesh(Skeleton* skeleton)
{
	if (!skeleton || !skeleton->m_root)
		return;
	BuildSkeletonJointMesh(skeleton->m_root);
}

Skeleton * D3D11Renderer::CreateSkeletonMesh(const std::string & filePath)
{
	Skeleton* skeleton = new Skeleton();
	FBXUtils::LoadSkeletonMesh(filePath, skeleton);
	BuildSkeletonMesh(skeleton);
	return skeleton;
}

Skeleton * D3D11Renderer::GetSkeletonMesh(const std::string & filePath)
{
	return m_skeletons[filePath];
}

Skeleton * D3D11Renderer::CreateOrGetSkeletonMesh(const std::string & filePath)
{
	auto found = m_skeletons.find(filePath);
	if (found == m_skeletons.end())
		return CreateSkeletonMesh(filePath);
	return GetSkeletonMesh(filePath);
}

void D3D11Renderer::BuildSkeletonJointMesh(SkeletonJoint* joint)
{
	if (!joint || !joint->m_builder)
		return;
	SAFE_DELETE(joint->m_mesh);
	joint->m_mesh = BuildMesh(*joint->m_builder);
	for (auto child : joint->m_children)
		BuildSkeletonJointMesh(child);
}

Mesh * D3D11Renderer::GetMesh(const std::string & filePath)
{
	return m_meshes[filePath];
}

void D3D11Renderer::DrawMesh(Mesh * mesh)
{
	if (mesh->m_useIndex)
		DrawIndexedVbo(mesh->m_primitiveType, mesh->m_vertexBuffer, mesh->m_indexBuffer);
	else
		DrawVbo(mesh->m_primitiveType, mesh->m_vertexBuffer);
}

void D3D11Renderer::DrawSkeleton(Skeleton* skeleton)
{
	if (!skeleton || !skeleton->m_root)
		return;
	DrawSkeletonJoint(skeleton->m_root);
}

void D3D11Renderer::DrawSkeletonJoint(SkeletonJoint* joint)
{
	PushMatrix();
	MultMatrix(joint->m_localMat);
	if(joint->m_mesh)
		DrawMesh(joint->m_mesh);
	for (auto child : joint->m_children)
		DrawSkeletonJoint(child);
	PopMatrix();
}
*/


#else
#endif


void D3D11Renderer::DrawVoxelModel(const VoxelModel* model)
{
	SetTexture(nullptr);
	DrawVbo(PRIMITIVE_TRIANGLES, model->m_vbo);
}



VoxelModel* D3D11Renderer::CreateOrGetVoxelModel(const std::string& filePath)
{
	VoxelModel* model = VoxelModel::CreateOrGetVoxelModel(filePath);
	model->FlipX();
	if (!model->m_vbo)
		model->m_vbo = CreateVertexBuffer(model->m_vertexes, PRIMITIVE_QUADS);
	return model;
}

void D3D11Renderer::SetMaterial(const Material* material)
{
	if (!material)
	{
		SetMaterial(m_defaultMaterial);
		return;
	}
	EnableBlend(material->m_srcBlend, material->m_destBlend);
	SetRasterState(material->m_cullMode, material->m_fillMode);
	SetShader(material->m_shaderName);
	SetDiffuseTexture(CreateOrGetTexture(material->m_diffuseTextureFilePath));
	SetNormalTexture(CreateOrGetTexture(material->m_normalTextureFilePath));
	SetSpecularTexture(CreateOrGetTexture(material->m_specularTextureFilePath));
}

void D3D11Renderer::SetMaterial(const std::string& materialName)
{
	Material* material;
	material = Material::CreateOrGetMaterial("Data/Material/" + materialName + ".xml");
	if (material)
		SetMaterial(material);
}

void D3D11Renderer::DrawParticleSystem(const Vector3& cameraPosition)
{
	ParticleSystem* instance = ParticleSystem::GetInstance();
	for (auto effect : instance->m_effects)
	{
		for (auto emitter : effect->m_emitters)
		{
			std::vector<Vertex3> vertices;
			/*if(emitter->m_definition->m_particleType == ParticleEmitterDefinition::DEFAULT)
				emitter->SortByDistance(cameraPosition);*/
			emitter->GetVertices(vertices, cameraPosition);
			VertexBuffer* vbo = CreateVertexBuffer(vertices, PRIMITIVE_TRIANGLES, false, false);
			SetMaterial(emitter->m_definition->m_material);
			DrawVbo(PRIMITIVE_TRIANGLES,vbo);
			delete vbo;
		}
	}
	SetMaterial(nullptr);
}

void D3D11Renderer::SetupConsole()
{
	std::string fontName;
	Config::GetString(fontName, "font");
	Console::s_instance->m_font = Font::CreateFontFromFile(m_rhiDevice,fontName);

	float windowHeight = (float)GetWindowSize().y;
	Console::SetScale(Console::s_instance->m_font->CalcScaleByFontHeight(windowHeight*0.05f));
	float lineHeight = Console::s_instance->m_font->m_lineHeight * Console::s_instance->m_scale;
	if (lineHeight != 0)
		Console::s_instance->m_maxNumOfLogs = (int)(windowHeight / lineHeight);
}

void D3D11Renderer::DrawConsole()
{
	PROFILE_FUNCTION;
	Console* console = Console::s_instance;

	EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	SetShader(nullptr);
	IntVector2 windowSize = GetWindowSize();
	SetOrthoProjection((float)windowSize.x, (float)windowSize.y);
	EnableDepthWrite(false);

	Rgba consoleBgColor = Rgba::WHITE;
	consoleBgColor.ScaleRGB(0.5f);
	consoleBgColor.ScaleAlpha(0.8f);

	Rgba consoleInputColor = Rgba::WHITE;
	consoleInputColor.ScaleRGB(0.2f);
	consoleInputColor.ScaleAlpha(0.8f);

	float inputLineHeight = 1.3f * console->m_font->m_lineHeight * console->m_scale;
	float totalLineHeight = console->m_font->m_lineHeight * (1.f + console->m_logs.size()) * console->m_scale;

	SetTexture(nullptr);
	Vertex3 bgVertices[8] = {
		Vertex3(Vector3(0.f,0.f,0.f),consoleInputColor),
		Vertex3(Vector3((float)windowSize.x,0.f,0.f),consoleInputColor),
		Vertex3(Vector3((float)windowSize.x,inputLineHeight,0.f),consoleInputColor),
		Vertex3(Vector3(0.f,inputLineHeight,0.f),consoleInputColor),

		Vertex3(Vector3(0.f,inputLineHeight,0.f),consoleBgColor),
		Vertex3(Vector3((float)windowSize.x,inputLineHeight,0.f),consoleBgColor),
		Vertex3(Vector3((float)windowSize.x,totalLineHeight,0.f),consoleBgColor),
		Vertex3(Vector3(0.f,totalLineHeight,0.f),consoleBgColor),
	};
	VertexBuffer* bg = CreateVertexBuffer(bgVertices, 8, PRIMITIVE_QUADS);
	DrawVbo(PRIMITIVE_TRIANGLES, bg);
	SAFE_DELETE(bg);



	Vector2 pringPosition = console->m_topLeft;
	pringPosition = Vector2(windowSize.x * 0.01f, windowSize.y * 0.00f);
	pringPosition = Vector2::ZERO;

	std::vector<Vertex3> vertices;
	SetMaterial("Text");
	SetDiffuseTexture(Console::s_instance->m_font->m_texture);
	vertices = MeshBuilder::Text2D(pringPosition, Console::s_instance->m_scale, Rgba::WHITE, Console::s_instance->m_font, console->m_inputStream + "\n", TEXTALIGN_HORIZONTAL_LEFT, TEXTALIGN_VERTICAL_BOTTOM);
	//DrawText2D(pringPosition, 2.f, Rgba::WHITE, console->m_font, console->m_inputStream);
	//pringPosition.y += console->m_font->m_lineHeight * console->m_scale;
	DrawVertices(vertices);
	for (size_t logIndex = console->m_logShowIndexOffset; 
		logIndex < console->m_logs.size() && logIndex < console->m_maxNumOfLogs + console->m_logShowIndexOffset; 
		logIndex++)
	{
		std::pair<Rgba, std::string>& log = console->m_logs[logIndex];
		vertices = MeshBuilder::Text2D(pringPosition, Console::s_instance->m_scale, log.first, Console::s_instance->m_font, log.second + "\n", TEXTALIGN_HORIZONTAL_LEFT, TEXTALIGN_VERTICAL_BOTTOM);
		DrawVertices(vertices);
		//DrawText2D(pringPosition, 2.f, log.first, console->m_font, log.second);
		size_t lineCount = StringUtils::CountToken(log.second, '\n');
		//pringPosition.y += console->m_font->m_lineHeight * console->m_scale;
	}
	DisableBlend();
}

void D3D11Renderer::DrawProfiler()
{
	if (!Console::s_instance)
		return;
	PROFILE_FUNCTION;
	Font* font = Console::s_instance->m_font;

	//MemoryProfiler* memory = MemoryProfiler::GetInstance();

	EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	SetShader(nullptr);
	IntVector2 windowSize = GetWindowSize();
	SetOrthoProjection((float)windowSize.x, (float)windowSize.y);
	EnableDepthWrite(false);

	Rgba consoleBgColor = Rgba::WHITE;
	consoleBgColor.ScaleRGB(0.5f);
	consoleBgColor.ScaleAlpha(0.8f);

	Rgba consoleInputColor = Rgba::WHITE;
	consoleInputColor.ScaleRGB(0.2f);
	consoleInputColor.ScaleAlpha(0.8f);

	SetTexture(nullptr);

	Vector2 printPosition = Vector2(m_rhiOutput->m_window->GetWindowSize());
	printPosition.y -= printPosition.x * 0.05f;
	printPosition.x *= 0.05f;

	std::string string;

#if TRACK_MEMORY != TRACK_MEMORY_NONE

	string += "MEMORY ";

#if TRACK_MEMORY == TRACK_MEMORY_BASIC

	string += "[BASIC]\n";

#elif TRACK_MEMORY == TRACK_MEMORY_VERBOSE

	string += "[VERBOSE]\n";

#endif

	string += "Alloc Count      : " + std::to_string(MemoryProfiler::GetAllicationCount()) + "\n";
	string += "Bytes Allocated  : " + MemoryProfiler::ToByteSizeString(MemoryProfiler::GetMemoryByte()) + "\n";
	string += "High Water       : " + MemoryProfiler::ToByteSizeString(MemoryProfiler::GetHighWater()) + "\n";
	string += "Alloc last Frame : " + std::to_string(MemoryProfiler::GetFrameAllocationCount()) + "\n";
	string += "Free last Frame  : " + std::to_string(MemoryProfiler::GetFrameFreeCount()) + "\n";

	string += "\n\n";

#endif

#ifdef PROFILER_ENABLED

	ProfilerReport profilerReport;

	if (Profiler::IsTreeView())
		profilerReport.CreateTreeView();
	else
		profilerReport.CreateFlatView();

	string += profilerReport.ToString();

#endif // PROFILER_ENABLED

	std::vector<Vertex3> vertices = MeshBuilder::Text2D(printPosition, 1.f, Rgba::WHITE, font, string);

	if(!vertices.empty())
	{
		PROFILE_SCOPE("DrawProfiler: Create and draw vbo");

		//VertexBuffer* vbo = CreateVertexBuffer(vertices, PRIMITIVE_QUADS, false, false);
		VertexBuffer* vbo = CreateVertexBuffer(vertices, PRIMITIVE_TRIANGLES, false, false);
		SetTexture(font->m_texture);
		DrawVbo(PRIMITIVE_TRIANGLES, vbo);
		SAFE_DELETE(vbo);
	}
	
	
	DisableBlend();
}

void D3D11Renderer::SetComputeShader(ComputeShader * shader)
{
	m_rhiContext->SetComputeShader(shader);
}

void D3D11Renderer::DispatchComputeJob(ComputeJob* computeJob)
{
	m_rhiContext->Dispatch(computeJob);
}

void D3D11Renderer::SetUnorderedAccessViews(unsigned int slotIndex, Texture2D* texture)
{
	m_rhiContext->SetUnorderedAccessViews(slotIndex,texture);
}

void D3D11Renderer::SetComputeConstantBuffer(unsigned int slotIndex, ConstantBuffer* constantBuffer)
{
	m_rhiContext->SetComputeConstantBuffer(slotIndex, constantBuffer);
}

Texture2D* D3D11Renderer::CreateOrGetTexture(const std::string& filePath)
{
	std::map<std::string, Texture2D*>::iterator found = m_textures.find(filePath);
	if (found != m_textures.end())
	{
		return GetTexture(filePath);
	}
	else
	{
		return CreateTexture(filePath);
	}
}

void D3D11Renderer::RotateX(float rotationX)
{
	m_modelMatrices.top().RotateDegreesAboutX(rotationX);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::RotateY(float rotationY)
{
	m_modelMatrices.top().RotateDegreesAboutY(rotationY);
	SetModelMatrix(m_modelMatrices.top());
}

void D3D11Renderer::RotateZ(float rotationZ)
{
	m_modelMatrices.top().RotateDegreesAboutZ(rotationZ);
	SetModelMatrix(m_modelMatrices.top());
}

IntVector2 D3D11Renderer::GetWindowSize() const
{
	return m_rhiOutput->m_window->GetWindowSize();
}

void D3D11Renderer::SetPerspectiveProjection(float fieldOfView, float ratio, float nearZ, float farZ)
{
	Matrix4 projection = Matrix4::MatrixMakePerspectiveProjection(fieldOfView, ratio, nearZ, farZ);
	SetProjectionMatrix(projection);
}

void D3D11Renderer::SetProjectionMatrix(const Matrix4& projection)
{
	m_matrix.projection = projection;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

Texture2D * D3D11Renderer::GetTexture(const std::string & filePath)
{
	return m_textures[filePath];
}

Texture2D * D3D11Renderer::CreateTexture(const std::string & filePath)
{
	Texture2D* texture = new Texture2D(m_rhiDevice, filePath);
	m_textures[filePath] = texture;
	return texture;
}

