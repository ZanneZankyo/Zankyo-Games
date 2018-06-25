#include "Mesh.hpp"
#include "Engine\File\Persistence.hpp"
#include <algorithm>
#include "..\Core\Console.hpp"
#include "MeshBuilder.hpp"
#include "..\Tools\fbx.hpp"

std::map<std::string, Mesh*> Mesh::s_loadedMeshes;
std::map<std::string, std::vector<Mesh*>*> Mesh::s_loadedMeshSets;

Mesh::Mesh(PrimitiveType primitiveType)
	: m_primitiveType(primitiveType)
	, m_vertices()
	, m_vertexBuffer()
	, m_useIndex()
	, m_indices()
	, m_indexBuffer()
	, m_material(nullptr)
	, m_name()
	, m_blendShape()
{
}


Mesh::Mesh(
	PrimitiveType primitiveType, 
	const std::string& name, 
	const std::vector<Vertex3>& vertices, 
	VertexBuffer* vertexBuffer, 
	Material* material /*= nullptr*/, 
	bool useIndex /*= false*/, 
	const std::vector<uint32_t>& indices /*= std::vector<uint32_t>()*/, 
	IndexBuffer* indexBuffer /*= nullptr*/,
	const BlendShape& blendShape
)
	: m_primitiveType(primitiveType)
	, m_vertices(vertices)
	, m_vertexBuffer(vertexBuffer)
	, m_useIndex(useIndex)
	, m_indices(indices)
	, m_indexBuffer(indexBuffer)
	, m_material(material)
	, m_name(name)
	, m_blendShape(blendShape)
{}

size_t Mesh::ByteVolumn() const
{
	size_t volumn = 0;
	volumn += sizeof(m_primitiveType);
	volumn += sizeof(m_useIndex);
	volumn += sizeof(m_vertices.size());
	volumn += sizeof(Vertex3) * m_vertices.size();
	volumn += sizeof(char) * m_name.size() + 1;
	if (m_useIndex)
	{
		volumn += sizeof(m_indices.size());
		volumn += sizeof(unsigned int) * m_indices.size();
	}
	volumn += sizeof(size_t) + m_blendShape.m_name.size() + 1;
	for (auto& channel : m_blendShape.m_channels)
	{
		volumn += sizeof(size_t) + channel.m_name.size() + 1;
		volumn += sizeof(BlendShapeParameter) * channel.m_points.size();
	}
	return volumn;
}

Mesh* Mesh::GetOrLoad(const std::string& filePath)
{
	//std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
	auto found = s_loadedMeshes.find(filePath);
	if (found != s_loadedMeshes.end())
		return found->second;
	return Load(filePath);
}

Mesh * Mesh::Load(const std::string & filePath)
{
	std::vector<byte_t> buffer;

	bool loadFileResult = Persistence::LoadBinaryFileToBuffer(filePath, buffer);
	if (!loadFileResult)
		return nullptr;
	size_t bufferOffset = 0;
	
	PrimitiveType primitiveType = Persistence::ReadBuffer<PrimitiveType>(buffer, bufferOffset);
	bool useIndex = Persistence::ReadBuffer<bool>(buffer, bufferOffset);
	std::vector<Vertex3> vertices;
	size_t numOfVertices = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
	vertices.reserve(numOfVertices);
	for (size_t vertexCount = 0; vertexCount < numOfVertices; vertexCount++)
	{
		vertices.push_back(Persistence::ReadBuffer<Vertex3>(buffer, bufferOffset));
	}
	std::vector<uint32_t> indices;
	if (useIndex)
	{
		size_t numOfIndices = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		indices.reserve(numOfIndices);
		for (size_t indexCount = 0; indexCount < numOfIndices; indexCount++)
		{
			indices.push_back(Persistence::ReadBuffer<uint32_t>(buffer, bufferOffset));
		}
	}
	size_t blendShapeNameLength = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
	std::string blendShapeName;
	blendShapeName.reserve(blendShapeNameLength + 1);
	for (size_t charIndex = 0; charIndex < blendShapeNameLength; charIndex++)
		blendShapeName.push_back(Persistence::ReadBuffer<char>(buffer, bufferOffset));
	BlendShape blendshape(blendShapeName);
	size_t blendShapeChannelSize = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
	for (unsigned int shapeIndex = 0; shapeIndex < blendShapeChannelSize; shapeIndex++)
	{
		size_t blendShapeChannelNameLength = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		std::string blendShapeChannelName;
		blendShapeChannelName.reserve(blendShapeChannelNameLength + 1);
		for (size_t charIndex = 0; charIndex < blendShapeChannelNameLength; charIndex++)
			blendShapeChannelName.push_back(Persistence::ReadBuffer<char>(buffer, bufferOffset));
		size_t blendShapeChannelVertSize = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		BlendShapeChannel channel(blendShapeChannelName);
		for (unsigned int shapeVertIdx = 0; shapeVertIdx < blendShapeChannelVertSize; shapeVertIdx++)
		{
			Vector3 position = Persistence::ReadBuffer<Vector3>(buffer, bufferOffset);
			Vector3 normal = Persistence::ReadBuffer<Vector3>(buffer, bufferOffset);
			channel.AddPoint(BlendShapeParameter(Vector4(position), Vector4(normal)));
		}
		blendshape.AddChannel(channel);
	}

	Mesh* mesh = new Mesh(primitiveType, "", vertices, nullptr, nullptr, useIndex, indices, nullptr, blendshape);
	s_loadedMeshes[filePath] = mesh;
	return mesh;
}

bool Mesh::Save(const std::string& filePath, const Mesh* mesh)
{
	std::vector<byte_t> buffer;
	buffer.reserve(mesh->ByteVolumn());
	Persistence::PushBuffer(mesh->m_primitiveType, buffer, true);
	Persistence::PushBuffer(mesh->m_useIndex, buffer, true);
	Persistence::PushBuffer(mesh->m_vertices.size(), buffer, true);
	for (unsigned int verticesIndex = 0; verticesIndex < mesh->m_vertices.size(); verticesIndex++)
	{
		Persistence::PushBuffer(mesh->m_vertices[verticesIndex], buffer, true);
	}
	if (mesh->m_useIndex)
	{
		Persistence::PushBuffer(mesh->m_indices.size(), buffer, true);
		for (unsigned int indexIndex = 0; indexIndex < mesh->m_indices.size(); indexIndex++)
		{
			Persistence::PushBuffer(mesh->m_indices[indexIndex], buffer, true);
		}
	}

	Persistence::PushBuffer(mesh->m_blendShape.m_name.data(), mesh->m_blendShape.m_name.size(), buffer, true);
	Persistence::PushBuffer(mesh->m_blendShape.m_channels.size(), buffer, true);
	for (unsigned int shapeIndex = 0; shapeIndex < mesh->m_blendShape.m_channels.size(); shapeIndex++)
	{
		const BlendShapeChannel& shape = mesh->m_blendShape.m_channels[shapeIndex];
		Persistence::PushBuffer(shape.m_name.data(), shape.m_name.size(), buffer, true);
		for (unsigned int shapeVertIdx = 0; shapeVertIdx < shape.m_points.size(); shapeVertIdx++)
		{
			Persistence::PushBuffer(Vector3(shape.m_points[shapeVertIdx].position), buffer, true);
			Persistence::PushBuffer(Vector3(shape.m_points[shapeVertIdx].normal), buffer, true);
		}
	}
	return Persistence::SaveBufferToBinaryFile(filePath, buffer);
}

std::vector<Mesh*>* Mesh::GetOrLoadSet(const std::string& filePath)
{
	auto found = s_loadedMeshSets.find(filePath);
	if (found != s_loadedMeshSets.end())
		return found->second;
	return LoadSet(filePath);
}

bool Mesh::SaveSet(const std::string& filePath, const std::vector<Mesh*>* meshes)
{
	std::vector<byte_t> buffer;
	size_t totalSize = 0;
	for (auto mesh : *meshes)
		totalSize += mesh->ByteVolumn();
	buffer.reserve(totalSize + sizeof(size_t));
	Persistence::PushBuffer(meshes->size(), buffer, true);
	for (auto mesh : *meshes)
	{
		Persistence::PushBuffer(mesh->m_name.size(), buffer, true);
		for(auto& nameChar : mesh->m_name)
			Persistence::PushBuffer(nameChar, buffer, true);
		Persistence::PushBuffer(mesh->m_primitiveType, buffer, true);
		Persistence::PushBuffer(mesh->m_useIndex, buffer, true);
		Persistence::PushBuffer(mesh->m_vertices.size(), buffer, true);
		for (unsigned int verticesIndex = 0; verticesIndex < mesh->m_vertices.size(); verticesIndex++)
		{
			Persistence::PushBuffer(mesh->m_vertices[verticesIndex], buffer, true);
		}
		if (mesh->m_useIndex)
		{
			Persistence::PushBuffer(mesh->m_indices.size(), buffer, true);
			for (unsigned int indexIndex = 0; indexIndex < mesh->m_indices.size(); indexIndex++)
			{
				Persistence::PushBuffer(mesh->m_indices[indexIndex], buffer, true);
			}
		}

		Persistence::PushBuffer(mesh->m_blendShape.m_name.data(), mesh->m_blendShape.m_name.size(), buffer, true);
		Persistence::PushBuffer(mesh->m_blendShape.m_channels.size(), buffer, true);
		for (unsigned int shapeIndex = 0; shapeIndex < mesh->m_blendShape.m_channels.size(); shapeIndex++)
		{
			const BlendShapeChannel& shape = mesh->m_blendShape.m_channels[shapeIndex];
			Persistence::PushBuffer(shape.m_name.data(), shape.m_name.size(), buffer, true);
			Persistence::PushBuffer(shape.m_points.size(), buffer, true);
			for (unsigned int shapeVertIdx = 0; shapeVertIdx < shape.m_points.size(); shapeVertIdx++)
			{
				Persistence::PushBuffer(shape.m_points[shapeVertIdx], buffer, true);
			}
		}
	}
	
	return Persistence::SaveBufferToBinaryFile(filePath, buffer);
}

std::vector<Mesh*>* Mesh::LoadSet(const std::string& filePath)
{

	std::vector<byte_t> buffer;
	bool loadFileResult = Persistence::LoadBinaryFileToBuffer(filePath, buffer);
	if (!loadFileResult)
		return nullptr;
	std::vector<Mesh*>* meshes = new std::vector<Mesh *>();
	size_t bufferOffset = 0;

	size_t numOfMeshes = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);

	for (size_t meshIndex = 0; meshIndex < numOfMeshes; meshIndex++)
	{
		std::string name;
		name = Persistence::ReadBufferAsString(buffer, bufferOffset);
		/*
		size_t nameLength = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		name.reserve(nameLength + 1);
		for (size_t charIndex = 0; charIndex < nameLength; charIndex++)
			name.push_back(Persistence::ReadBuffer<char>(buffer, bufferOffset));
		*/
		PrimitiveType primitiveType = Persistence::ReadBuffer<PrimitiveType>(buffer, bufferOffset);
		bool useIndex = Persistence::ReadBuffer<bool>(buffer, bufferOffset);
		std::vector<Vertex3> vertices;
		size_t numOfVertices = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		vertices.reserve(numOfVertices);
		for (size_t vertexCount = 0; vertexCount < numOfVertices; vertexCount++)
		{
			vertices.push_back(Persistence::ReadBuffer<Vertex3>(buffer, bufferOffset));
		}
		std::vector<uint32_t> indices;
		if (useIndex)
		{
			size_t numOfIndices = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
			indices.reserve(numOfIndices);
			for (size_t indexCount = 0; indexCount < numOfIndices; indexCount++)
			{
				indices.push_back(Persistence::ReadBuffer<uint32_t>(buffer, bufferOffset));
			}
		}
		std::string blendShapeName;
		blendShapeName = Persistence::ReadBufferAsString(buffer, bufferOffset);
		/*size_t blendShapeNameLength = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		blendShapeName.reserve(blendShapeNameLength + 1);
		for (size_t charIndex = 0; charIndex < blendShapeNameLength; charIndex++)
			blendShapeName.push_back(Persistence::ReadBuffer<char>(buffer, bufferOffset));*/
		BlendShape blendshape(blendShapeName);
		size_t blendShapeChannelSize = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
		for (unsigned int shapeIndex = 0; shapeIndex < blendShapeChannelSize; shapeIndex++)
		{
			std::string blendShapeChannelName;
			/*size_t blendShapeChannelNameLength = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
			blendShapeChannelName.reserve(blendShapeChannelNameLength + 1);
			for (size_t charIndex = 0; charIndex < blendShapeChannelNameLength; charIndex++)
				blendShapeChannelName.push_back(Persistence::ReadBuffer<char>(buffer, bufferOffset));*/
			blendShapeChannelName = Persistence::ReadBufferAsString(buffer, bufferOffset);
			size_t blendShapeChannelVertSize = Persistence::ReadBuffer<size_t>(buffer, bufferOffset);
			BlendShapeChannel channel(blendShapeChannelName);
			for (unsigned int shapeVertIdx = 0; shapeVertIdx < blendShapeChannelVertSize; shapeVertIdx++)
			{
				//Vector3 position = Persistence::ReadBuffer<Vector3>(buffer, bufferOffset);
				//Vector3 normal = Persistence::ReadBuffer<Vector3>(buffer, bufferOffset);
				channel.AddPoint(Persistence::ReadBuffer<BlendShapeParameter>(buffer, bufferOffset));
			}
			blendshape.AddChannel(channel);
		}

		Mesh* mesh = new Mesh(primitiveType, name, vertices, nullptr, nullptr, useIndex, indices, nullptr, blendshape);
		meshes->push_back(mesh);
	}

	s_loadedMeshSets[filePath] = meshes;
	return meshes;
}

#ifdef TOOLS_BUILD
std::vector<Mesh*>* Mesh::LoadSetFromFBX(const std::string& filePath, Skeleton* skeleton)
{

	bool result = false;
	MeshBuilder mb;
	result = FBXUtils::LoadMesh(filePath, mb, skeleton);

	if (!result)
		return nullptr;

	std::vector<Mesh*> *meshes = new std::vector<Mesh *>();
	mb.OutputMeshesData(meshes);
	s_loadedMeshSets[filePath] = meshes;
	return meshes;
}
#endif
