#pragma once
#include <vector>
#include "Engine\Math\AABB3.hpp"
#include <map>
#include "Engine\Math\Vertex3.hpp"
#include "Engine\RHI\VertexBuffer.hpp"

class VoxelModel
{
public:

	VertexBuffer* m_vbo;
	std::vector<Vertex3> m_vertexes;
	int m_light;
	//unsigned int m_vboId;
	//size_t m_numOfVertexes;

	VoxelModel();
	VoxelModel(const VoxelModel& copy);
	~VoxelModel();

	explicit VoxelModel(std::vector<Vertex3>& vertexes);

	VoxelModel& operator = (const VoxelModel& copy);

	void FlipX();
	void SetNewLight(int newLight);
	//void Render() const;

	static std::map<std::string, VoxelModel*> s_savedVoxelModels;

	static void DeleteAllModel();
	static VoxelModel* CreateOrGetVoxelModel(const std::string& modelFilePath);
	//static VoxelModel* GetVoxelModel(const std::string& modelFilePath);
	static VoxelModel* LoadVoxelModel(const std::string& modelFilePath);
	static VoxelModel* LoadFromPly(const std::string& modelFilePath);
	static VoxelModel* LoadFromQubicleBinary(const std::vector<unsigned char>& buffer);
};