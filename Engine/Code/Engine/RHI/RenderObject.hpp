#pragma once

#include "RHITypes.hpp"
#include "Engine\Math\Vector3.hpp"
#include <vector>

class RenderObject
{
public:
	VertexBuffer* m_vbo;
	ShaderProgram* m_shader;
	Texture2D* m_texture;
	Vector3 m_worldPosition;
	Vector3 m_rotation;
	std::vector<RenderObject*> m_children;
public:
	void Render();
	void UpdateVbo(const VertexBuffer* vbo);
	void UpdateShader(const ShaderProgram* shader);
	void UpdateTexture(const Texture2D* texture);
};