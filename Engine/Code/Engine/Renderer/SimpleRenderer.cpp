#include "SimpleRenderer.hpp"
#include <d3d11.h>
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\RHI\Font.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Core\RGBAf.hpp"
#include "Engine\Math\MathUtils.hpp"

//#define DEBUG


SimpleRenderer::~SimpleRenderer()
{
	
}

void SimpleRenderer::Setup(unsigned int width, unsigned int height)
{

	RHIInstance *ri = RHIInstance::GetInstance();
	ri->CreateOutput(m_rhiDevice, m_rhiContext, m_rhiOutput, width, height);
	//m_rhiDevice->CreateShaderProgramFromHlslFile("Data/hlsl/nop_color.hlsl");
	
	m_defaultRasterState = new RasterState(m_rhiDevice);
	m_rhiContext->SetRasterState(m_defaultRasterState);

	m_defaultShader = CreateOrGetShader("NoEffect");
	m_textures[""] = new Texture2D(m_rhiDevice,Image());
	m_textures["normal_default"] = new Texture2D(m_rhiDevice, Image(Rgba(128,128,255)));

	m_matrixConstantBuffer = new ConstantBuffer(m_rhiDevice, &m_matrix, sizeof(m_matrix));
	SetConstantBuffer(0, m_matrixConstantBuffer);

	m_defaultDepthStencil = new Texture2D(m_rhiDevice, width, height, IMAGEFORMAT_D24S8);
	m_currentDepthStencil = nullptr;

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

Window* SimpleRenderer::GetWindow()
{
	return m_rhiOutput->m_window;
}

void SimpleRenderer::Destroy()
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

bool SimpleRenderer::IsClosed()
{
	return m_rhiOutput->m_window->IsClosed();
}

void SimpleRenderer::SetRenderTarget(Texture2D *colorTarget, Texture2D *depthTarget /*= nullptr*/)
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

VertexBuffer * SimpleRenderer::CreateVertexBuffer(Vertex3* vertex, unsigned int numOfVertices, PrimitiveType topology /*= PRIMITIVE_TRIANGLES*/, bool addBack /*= false*/, bool overWriteNT /*= true*/)
{
	switch (topology)
	{
	case PRIMITIVE_QUADS:
	{
		std::vector<Vertex3> vertices;
		unsigned int bufferSize = (unsigned int)(numOfVertices * 1.5f * (addBack ? 2.f : 1.f));
		vertices.reserve(bufferSize);

		for (unsigned int offset = 0; offset < numOfVertices; offset += 4)
		{

			//LB -> RB -> RT -> LT

			/*if (overWriteNT)
			{
				Vector3 right = Interpolate<Vector3>(vertex[offset + 1].position - vertex[offset].position, vertex[offset + 2].position - vertex[offset + 3].position, 0.5f);
				Vector2 rightUvDelta = Interpolate<Vector2>(vertex[offset + 1].texCoords - vertex[offset].texCoords, vertex[offset + 2].texCoords - vertex[offset + 3].texCoords, 0.5f);
				if(rightUvDelta.x)
					right *= rightUvDelta.x;

				Vector3 up = Interpolate<Vector3>(vertex[offset + 3].position - vertex[offset].position, vertex[offset + 2].position - vertex[offset + 1].position, 0.5f);
				Vector2 upUvDelta = Interpolate<Vector2>(vertex[offset + 3].texCoords - vertex[offset].texCoords, vertex[offset + 2].texCoords - vertex[offset + 1].texCoords, 0.5f);
				if(upUvDelta.y)
					up *= -upUvDelta.y;

				Vector3 normal = -CrossProduct(right, up);//CrossProduct(up, right);
				Vector3 tangent = right;

				vertex[offset].normal = normal;
				vertex[offset].tangent = tangent;
				vertex[offset + 1].tangent = tangent;
				vertex[offset + 1].normal = normal;
				vertex[offset + 2].normal = normal;
				vertex[offset + 2].tangent = tangent;
				vertex[offset + 3].normal = normal;
				vertex[offset + 3].tangent = tangent;

			}*/
			
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
				vertices.push_back(Vertex3(vertex[offset + 1].position, vertex[offset + 1].color, vertex[offset + 1].texCoords, -vertex[offset + 1].normal, -vertex[offset +1 ].tangent));
				vertices.push_back(Vertex3(vertex[offset].position, vertex[offset].color, vertex[offset].texCoords, -vertex[offset].normal, -vertex[offset].tangent));
				vertices.push_back(Vertex3(vertex[offset + 3].position, vertex[offset + 3].color, vertex[offset + 3].texCoords, -vertex[offset + 3].normal, -vertex[offset + 3].tangent));
				vertices.push_back(Vertex3(vertex[offset + 2].position, vertex[offset + 2].color, vertex[offset + 2].texCoords, -vertex[offset + 2].normal, -vertex[offset + 2].tangent));
			}
		}

		if(overWriteNT)
			BuildTbn(vertices);

		return m_rhiDevice->CreateVertexBuffer(vertices.data(), vertices.size());
		break;
	}
	default:
		break;
	}

	
	return m_rhiDevice->CreateVertexBuffer(vertex, numOfVertices);
}

IndexBuffer * SimpleRenderer::CreateIndexBuffer(Vertex3* vertex, unsigned int numOfVertices, PrimitiveType topology /*= PRIMITIVE_TRIANGLES*/, bool addBack /*= false*/, bool overWriteNT /*= true*/)
{
	UNUSED(vertex);
	UNUSED(numOfVertices);
	UNUSED(topology);
	UNUSED(addBack);
	UNUSED(overWriteNT);
	return nullptr;
}

VertexBuffer * SimpleRenderer::CreateTbnVertexBuffer(Vertex3* vertex, unsigned int numOfVertices, PrimitiveType topology /*= PRIMITIVE_TRIANGLES*/)
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

void SimpleRenderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_rhiContext->SetViewports(x,y,width,height);
}

void SimpleRenderer::SetViewport()
{
	IntVector2 windowSize = m_rhiOutput->m_window->GetWindowSize();
	m_rhiContext->SetViewports(0, 0, windowSize.x, windowSize.y);
}

void SimpleRenderer::SetViewportAsPercent(float x, float y, float w, float h)
{
	UNUSED(x);
	UNUSED(y);
	UNUSED(w);
	UNUSED(h);
}

void SimpleRenderer::ClearColor(Rgba const & color)
{
	m_rhiContext->ClearColorTarget(m_currentTarget, color);
}

void SimpleRenderer::ClearColor(unsigned int colorCode)
{
	ClearColor(Rgba(colorCode));
}

void SimpleRenderer::ClearTargetColor(Texture2D * target, Rgba const & color)
{
	m_rhiContext->ClearColorTarget(target, color);
}

void SimpleRenderer::Present() const
{
	m_rhiOutput->Present();
}

bool SimpleRenderer::SetShader(ShaderProgram * shader)
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

bool SimpleRenderer::SetShader(const std::string& shaderName)
{
	ShaderProgram* shader = CreateOrGetShader(shaderName);
	return SetShader(shader);
}

ShaderProgram* SimpleRenderer::CreateOrGetShader(const std::string& shaderName)
{
	std::map<std::string, ShaderProgram*>::iterator found = m_shaders.find(shaderName);
	if (found != m_shaders.end())
		return GetShader(shaderName);
	else
	{
		ShaderProgram* shader = CreateShader(shaderName);
		return shader == nullptr ? m_defaultShader : shader;
	}
}

ShaderProgram* SimpleRenderer::GetShader(const std::string& shaderName)
{
	std::map<std::string, ShaderProgram*>::iterator found = m_shaders.find(shaderName);
	if (found != m_shaders.end())
		return found->second;
	else
		return nullptr;
}

ShaderProgram * SimpleRenderer::CreateShader(const std::string& shaderName)
{
	ShaderProgram* shader = m_rhiDevice->CreateShaderProgramFromHlslFile("Data/hlsl/" + shaderName + ".hlsl");
	//ASSERT_OR_DIE(shader, "Shader compile error!");
	if (shader == nullptr)
		return nullptr;
	m_shaders[shaderName] = shader;
	return shader;
}

void SimpleRenderer::BuildTbn(std::vector<Vertex3>& vertices)
{
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

void SimpleRenderer::DrawVbo(PrimitiveType topology, VertexBuffer * vbo)
{
	
	m_rhiContext->SetPrimitiveTopology(topology);

	m_rhiContext->SetVertexBuffers(vbo);

	m_rhiContext->Draw(vbo->m_numOfVertices, 0);
	
}

void SimpleRenderer::DrawVbo(PrimitiveType topology, std::vector<VertexBuffer>& vbo)
{
	UNUSED(topology);
	UNUSED(vbo);
	//Draw(topology, vbo.data(), vbo.size());
}

void SimpleRenderer::DrawIndexedVbo(PrimitiveType topology, const std::vector<IndexBuffer*>& ibo)
{
	UNUSED(topology);
	UNUSED(ibo);
}

void SimpleRenderer::DrawIndexedVbo(PrimitiveType topology, const IndexBuffer & indexedBuffer)
{
}

void SimpleRenderer::DrawText2D(Vector2 pos, float scale, Rgba color, Font* font, std::string string, int cursorIndex) // draw cursor when cursorIndex >= 0
{

	UNUSED(cursorIndex);
	UNUSED(scale);

	Vector2 currentPos = pos;
	Vector2 lineStartPos = pos;

	std::vector<Vertex3> vertices;

	for (size_t stringIndex = 0; stringIndex < string.size(); stringIndex++)
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
	}

	SetTexture(font->m_texture);
	VertexBuffer* vbo = CreateVertexBuffer(vertices.data(), vertices.size(), PRIMITIVE_QUADS);
	DrawVbo(PRIMITIVE_TRIANGLES, vbo);
	SAFE_DELETE(vbo);
}

void SimpleRenderer::SetSampler(Sampler * sampler, unsigned int textureIndex)
{
	m_rhiContext->m_dxContext->VSSetSamplers(textureIndex, 1, &sampler->m_dxSampler);
	m_rhiContext->m_dxContext->PSSetSamplers(textureIndex, 1, &sampler->m_dxSampler);
}

void SimpleRenderer::SetSampler(FilterMode minFilter, FilterMode magFilter)
{
	if (m_sampler)
		SAFE_DELETE(m_sampler);
	m_sampler = new Sampler(m_rhiDevice, minFilter, magFilter);
	SetSampler(m_sampler);
}

void SimpleRenderer::SetTexture(Texture2D * texture, unsigned int textureIndex)
{
	if (texture == nullptr)
		texture = m_textures[""];

	m_rhiContext->SetShaderResources(textureIndex, texture);

	m_rhiContext->m_dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
	m_rhiContext->m_dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxSrv);
}

void SimpleRenderer::SetConstantBuffer(unsigned idx, ConstantBuffer * constantBuffer)
{
	m_rhiContext->m_dxContext->VSSetConstantBuffers(idx, 1, &constantBuffer->m_dxBuffer);
	m_rhiContext->m_dxContext->PSSetConstantBuffers(idx, 1, &constantBuffer->m_dxBuffer);
}

void SimpleRenderer::EnableBlend(BlendFactor src, BlendFactor dest)
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

void SimpleRenderer::DisableBlend()
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

void SimpleRenderer::EnableDepthTest(bool enabled /*= true*/)
{
	if (m_depthStencilDesc.depthTestEnabled != enabled) {
		m_depthStencilDesc.depthTestEnabled = enabled;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::EnableDepthWrite(bool enabled /*= true*/)
{
	if (m_depthStencilDesc.depthWritingEnabled != enabled) {
		m_depthStencilDesc.depthWritingEnabled = enabled;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::SetOrthoProjection(float width, float height)
{
	SetOrthoProjection(0.f, 0.f, width, height);
}

void SimpleRenderer::SetOrthoProjection(float nearX, float nearY, float farX, float farY)
{
	Matrix4 projection = Matrix4::MatrixMakeOrthoProjection(nearX, nearY, farX, farY);
	SetProjectionMatrix(projection);
	SetViewMatrix(Matrix4::CreateIdentity());
}

void SimpleRenderer::ClearDepth(float depth/* = 1.0f*/, uint8_t stencil/* = 0*/)
{
	m_rhiContext->ClearDepthTarget(m_currentDepthStencil, depth, stencil);
}

void SimpleRenderer::SetViewMatrix(const Matrix4& view)
{
	m_matrix.view = view;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void SimpleRenderer::SetModelMatrix(const Matrix4& matrix)
{
	m_matrix.model = matrix;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void SimpleRenderer::SetAmbientLight(const Rgba& color, float intensity /*= 1.f*/)
{
	RgbaF colorF(color);
	Vector4 colorFactor(colorF.red, colorF.green, colorF.blue, intensity);
	if (colorFactor == m_lightBuffer.AMBIENT_FACTOR)
		return;
	m_lightBuffer.AMBIENT_FACTOR = colorFactor;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void SimpleRenderer::EnablePointLight(int lightIndex, const Vector3& position, const Rgba& color, float intensity, const Vector4& attenuation, const Vector4& specAttenuation)
{
	RgbaF colorF(color);
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_POSITION = Vector4(position);
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_COLOR = Vector4(colorF.red, colorF.green, colorF.blue, intensity);
	m_lightBuffer.POINT_LIGHTS[lightIndex].ATTENUATION = attenuation;
	m_lightBuffer.POINT_LIGHTS[lightIndex].SPEC_ATTENUATION = specAttenuation;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void SimpleRenderer::DisablePointLight(int lightIndex)
{
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_POSITION = Vector4::ZERO;
	m_lightBuffer.POINT_LIGHTS[lightIndex].LIGHT_COLOR = Vector4::ZERO;
	m_lightBuffer.POINT_LIGHTS[lightIndex].ATTENUATION = Vector4::ZERO;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void SimpleRenderer::DisableAllPointLight()
{
	for (int i = 0; i < 8; i++)
		m_lightBuffer.POINT_LIGHTS[i] = PointLight();
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void SimpleRenderer::PopMatrix()
{
	if(m_modelMatrices.size()>1)
		m_modelMatrices.pop();
	SetModelMatrix(m_modelMatrices.top());
}

void SimpleRenderer::PushMatrix()
{
	m_modelMatrices.push(m_modelMatrices.top());
	SetModelMatrix(m_modelMatrices.top());
}

void SimpleRenderer::Translate(const Vector3& translate)
{
	m_modelMatrices.top().Translate(translate);
	SetModelMatrix(m_modelMatrices.top());
}

void SimpleRenderer::Translate3D(const Vector3 & translate)
{
	Translate(translate);
}

void SimpleRenderer::Translate3D(float x, float y, float z)
{
	Translate(Vector3(x, y, z));
}

void SimpleRenderer::SetDiffuse(Texture2D* texDiffuse)
{
	if (!texDiffuse)
		texDiffuse = m_textures[""];
	SetTexture(texDiffuse, 0);
}

void SimpleRenderer::SetNormalTexture(Texture2D* texNormal)
{
	if (!texNormal)
		texNormal = m_textures["normal_default"];
	SetTexture(texNormal, 1);
}

void SimpleRenderer::SetSpecularTexture(Texture2D* texSpec)
{
	if (!texSpec)
		texSpec = m_textures[""];
	SetTexture(texSpec, 2);
}

void SimpleRenderer::SetEyePosition(const Vector3& eyePosition)
{
	m_matrix.eyePosition = Vector4(eyePosition,1.f);
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

void SimpleRenderer::SetSpecularConstants(float specPower, float specFactor)
{
	m_lightBuffer.SPEC_POWER = specPower;
	m_lightBuffer.SPEC_FACTOR = specFactor;
	m_lightConstantBuffer->Update(m_rhiContext, &m_lightBuffer);
}

void SimpleRenderer::AddDebugLines(const Vector3 & start, const Vector3 & end, const Rgba & color)
{
	m_debugLines.push_back(Vertex3(start,color));
	m_debugLines.push_back(Vertex3(end, color));
}

void SimpleRenderer::DrawDebugLines()
{
	VertexBuffer* debugVbo = CreateVertexBuffer(m_debugLines.data(), m_debugLines.size(), PRIMITIVE_LINES);
	DrawVbo(PRIMITIVE_LINES, debugVbo);
	SAFE_DELETE(debugVbo);
}

void SimpleRenderer::ClearDebugLines()
{
	m_debugLines.clear();
}

void SimpleRenderer::Scale(const Vector3& scale)
{
	m_modelMatrices.top().Scale(scale);
	SetModelMatrix(m_modelMatrices.top());
}

void SimpleRenderer::Rotate(float rotationDegrees, const Vector3& axis)
{
	m_modelMatrices.top().RotateDegreesAboutAxis(rotationDegrees, axis);
	SetModelMatrix(m_modelMatrices.top());
}

void SimpleRenderer::Rotate3D(float rotationDegrees, const Vector3& axis)
{
	Rotate(rotationDegrees, axis);
}

void SimpleRenderer::GenerateVBOBuffer(int numOfVbos, unsigned int* out_vboIndex)
{
	
}

void SimpleRenderer::Scale3D(float scale)
{
	Scale(Vector3(scale, scale, scale));
}

void SimpleRenderer::SetRaster(bool frontCounterClockwise /*= true*/)
{
	SAFE_DELETE(m_defaultRasterState);
	m_defaultRasterState = new RasterState(m_rhiDevice, frontCounterClockwise);
	m_rhiContext->SetRasterState(m_defaultRasterState);
}

Texture2D* SimpleRenderer::CreateOrGetTexture(const std::string& filePath)
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

void SimpleRenderer::RotateY(float rotationY)
{
	m_modelMatrices.top().RotateDegreesAboutY(rotationY);
	SetModelMatrix(m_modelMatrices.top());
}

IntVector2 SimpleRenderer::GetWindowSize() const
{
	return m_rhiOutput->m_window->GetWindowSize();
}

void SimpleRenderer::SetPerspectiveProjection(float fieldOfView, float ratio, float nearZ, float farZ)
{
	Matrix4 projection = Matrix4::MatrixMakePerspectiveProjection(fieldOfView, ratio, nearZ, farZ);
	SetProjectionMatrix(projection);
}

void SimpleRenderer::SetProjectionMatrix(const Matrix4& projection)
{
	m_matrix.projection = projection;
	m_matrixConstantBuffer->Update(m_rhiContext, &m_matrix);
}

Texture2D * SimpleRenderer::GetTexture(const std::string & filePath)
{
	return m_textures[filePath];
}

Texture2D * SimpleRenderer::CreateTexture(const std::string & filePath)
{
	Texture2D* texture = new Texture2D(m_rhiDevice, filePath);
	m_textures[filePath] = texture;
	return texture;
}

