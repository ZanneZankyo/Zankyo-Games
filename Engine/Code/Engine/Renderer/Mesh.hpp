#pragma once

#include "Engine/RHI/RHITypes.hpp"
#include <vector>
#include "Engine/Math/Vertex3.hpp"
#include <map>
#include "Engine/RHI/Material.hpp"
#include <string>
#include "BlendShape.hpp"
#include "Skeleton.hpp"

struct DrawInstruction 
{
	unsigned int startIndex;  
	unsigned int count;  
	unsigned int type;  
	bool usesIndexBuffer;

	DrawInstruction(unsigned int p_startIndex = 0, unsigned int p_count = 0, unsigned int p_type = 0, bool p_usesIndexBuffer = false)
		: startIndex(p_startIndex)
		, count(p_count)
		, type(p_type)
		, usesIndexBuffer(p_usesIndexBuffer)
	{}
};

class Mesh
{
public:
	Mesh(PrimitiveType primitiveType = PRIMITIVE_TRIANGLES);
	Mesh(PrimitiveType primitiveType, const std::string& name, 
		const std::vector<Vertex3>& vertices, VertexBuffer* vertexBuffer, Material* material = nullptr, 
		bool useIndex = false, const std::vector<uint32_t>& indices = std::vector<uint32_t>(), IndexBuffer* indexBuffer = nullptr,
		const BlendShape& blendShape = BlendShape()
	);
public:
	std::vector<Vertex3> m_vertices;
	std::vector<uint32_t> m_indices;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	bool m_useIndex;
	PrimitiveType m_primitiveType;
	Material* m_material;
	std::string m_name;
	BlendShape m_blendShape;

	size_t ByteVolumn() const;

	static Mesh* GetOrLoad(const std::string& filePath);
	static Mesh* Load(const std::string& filePath);
	static bool Save(const std::string& filePath, const Mesh* mesh);
	static std::vector<Mesh*>* GetOrLoadSet(const std::string& filePath);
	static std::vector<Mesh*>* LoadSet(const std::string& filePath);
	static std::vector<Mesh*>* LoadSetFromFBX(const std::string& filePath, Skeleton* skeleton);
	static bool SaveSet(const std::string& filePath, const std::vector<Mesh*>* meshes);

	static std::map<std::string, Mesh*> s_loadedMeshes;
	static std::map<std::string, std::vector<Mesh*>*> s_loadedMeshSets;
};