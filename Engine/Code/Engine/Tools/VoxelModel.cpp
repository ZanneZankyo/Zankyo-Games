#include "VoxelModel.hpp"
#include "Engine\File\Persistence.hpp"
#include <string.h>
//#include "Engine\Renderer\OpenGLExtensions.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include <fstream>

STATIC std::map<std::string, VoxelModel*> VoxelModel::s_savedVoxelModels;

VoxelModel::VoxelModel()
	: m_vertexes()
	, m_light(15)
	, m_vbo(nullptr)
{
}

VoxelModel::VoxelModel(const VoxelModel & copy)
	: m_vertexes(copy.m_vertexes)
	, m_light(copy.m_light)
	, m_vbo(nullptr)
{
	//m_vbo = g_renderer->CreateVertexBuffer(m_vertexes.data(), m_vertexes.size(), PRIMITIVE_QUADS);
}

VoxelModel::VoxelModel(std::vector<Vertex3>& vertexes)
	: m_vertexes(vertexes)
	, m_light(15)
	, m_vbo(nullptr)
{
	for (Vertex3& vertex : m_vertexes)
	{
		vertex.color.alpha = 255;
	}
	//m_vbo = g_renderer->CreateVertexBuffer(m_vertexes.data(), m_vertexes.size(), PRIMITIVE_QUADS, false, true);
}

VoxelModel::~VoxelModel()
{
	SAFE_DELETE(m_vbo);
}

VoxelModel & VoxelModel::operator=(const VoxelModel & copy)
{
	m_vertexes = copy.m_vertexes;
	m_light = copy.m_light;
	//m_vbo = g_renderer->CreateVertexBuffer(m_vertexes.data(), m_vertexes.size(), PRIMITIVE_QUADS);
	return *this;
}

void VoxelModel::FlipX()
{
	for (auto& vertex : m_vertexes)
	{
		vertex.position.x *= -1.f;
		vertex.normal.x *= -1.f;
	}
}

void VoxelModel::SetNewLight(int newLight)
{
	if (newLight <= 0)
		newLight = 1;
	if (newLight > 15)
		newLight = 15;
	m_light = newLight;
	/*if (newLight == 0)
		newLight = 1;
	for (Vertex3& vertex : m_vertexes)
	{
		vertex.color.ScaleRGB((float)newLight / (float)m_light);
	}
	SAFE_DELETE(m_vbo);
	m_vbo = g_renderer->CreateVertexBuffer(m_vertexes.data(), m_vertexes.size(), PRIMITIVE_QUADS);
	m_light = newLight;*/
}

/*
void VoxelModel::Render() const
{
	//g_renderer->DrawQuads3D(m_vboId,m_numOfVertexes);
	//g_renderer->DrawQuads3D(m_vertexes.data(), m_vertexes.size());

	if (g_theGame->m_ui->m_printDebug)
		g_renderer->EnableDepthTest(false);

	g_renderer->SetTexture(nullptr);
	g_vboColorScale.DRAW_COLOR_SCALE = m_light / 15.f;
	g_drawColor->Update(g_renderer->m_rhiContext, &g_vboColorScale);
	g_renderer->DrawVbo(PRIMITIVE_QUADS, m_vbo);
	g_vboColorScale.DRAW_COLOR_SCALE = 1.f;
	g_drawColor->Update(g_renderer->m_rhiContext, &g_vboColorScale);

	g_renderer->EnableDepthTest(true);
}*/

void VoxelModel::DeleteAllModel()
{
	for (std::pair<std::string, VoxelModel*> model : s_savedVoxelModels)
	{
		if (model.second)
			delete model.second;
	}
}

VoxelModel * VoxelModel::CreateOrGetVoxelModel(const std::string & modelFilePath)
{
	if (s_savedVoxelModels.find(modelFilePath) == s_savedVoxelModels.end())
	{
		s_savedVoxelModels[modelFilePath] = LoadVoxelModel(modelFilePath);
		return s_savedVoxelModels[modelFilePath];
	}
	else
		return s_savedVoxelModels[modelFilePath];
}

VoxelModel * VoxelModel::LoadVoxelModel(const std::string & modelFilePath)
{

	return LoadFromPly(modelFilePath);

	//std::vector<unsigned char> buffer;
	//Persistence::LoadBinaryFileToBuffer(modelFilePath, buffer);

}

VoxelModel * VoxelModel::LoadFromPly(const std::string & modelFilePath)
{
	std::ifstream in(modelFilePath);
	std::string readStream;
	int numOfVertexes = 0;
	int numOfFaces;
	do 
	{
		in >> readStream;
		if (readStream == "vertex")
			in >> numOfVertexes;
		if (readStream == "face")
			in >> numOfFaces;
	} while (readStream!="end_header");
	std::vector<Vertex3> vertexes;
	for (int vertexIndex = 0; vertexIndex < numOfVertexes; vertexIndex++)
	{
		Vector3 position;
		in >> position.x >> position.y >> position.z;
		int rgb[3];
		in >> rgb[0] >> rgb[1] >> rgb[2];
		Rgba color((unsigned char)rgb[0], (unsigned char)rgb[1], (unsigned char)rgb[2]);
		vertexes.push_back(Vertex3(position,color));
	}
	int numOfVerticsPerFace;
	for (int faceIndex = 0; faceIndex < numOfVertexes; faceIndex++)
	{
		
		in >> numOfVerticsPerFace;
		switch (numOfVerticsPerFace)
		{
		case 3:
		{
			int vertexIndex0, vertexIndex1, vertexIndex2;
			in >> vertexIndex0 >> vertexIndex1 >> vertexIndex2;
			Vector3 vector0 = vertexes[vertexIndex0].position;
			Vector3 vector1 = vertexes[vertexIndex1].position;
			Vector3 vector2 = vertexes[vertexIndex2].position;

			Vector3 normal = CrossProduct(vector2 - vector1, vector0 - vector1);

			vertexes[vertexIndex0].normal = normal;
			vertexes[vertexIndex1].normal = normal;
			vertexes[vertexIndex2].normal = normal;

			break;
		}
		case 4:
		{
			int vertexIndex0, vertexIndex1, vertexIndex2, vertexIndex3;
			in >> vertexIndex0 >> vertexIndex1 >> vertexIndex2 >> vertexIndex3;
			Vector3 vector0 = vertexes[vertexIndex0].position;
			Vector3 vector1 = vertexes[vertexIndex1].position;
			Vector3 vector2 = vertexes[vertexIndex2].position;
			Vector3 vector3 = vertexes[vertexIndex3].position;

			Vector3 normal1 = CrossProduct(vector2 - vector1, vector0 - vector1);
			Vector3 normal2 = CrossProduct(vector2 - vector3, vector0 - vector3);

			Vector3 normal = normal1;//Interpolate(normal1, normal2, 0.5f);

			Vector3 tangent1 = vector1 - vector0;
			Vector3 tangent2 = vector2 - vector3;

			Vector3 tangent = tangent1;//Interpolate(tangent1, tangent2, 0.5f);

			vertexes[vertexIndex0].normal = normal;
			vertexes[vertexIndex1].normal = normal;
			vertexes[vertexIndex2].normal = normal;
			vertexes[vertexIndex3].normal = normal;

			vertexes[vertexIndex0].tangent = tangent;
			vertexes[vertexIndex1].tangent = tangent;
			vertexes[vertexIndex2].tangent = tangent;
			vertexes[vertexIndex3].tangent = tangent;

			vertexes[vertexIndex0].texCoords = Vector2(0.f, 1.f);
			vertexes[vertexIndex1].texCoords = Vector2(1.f, 1.f);
			vertexes[vertexIndex2].texCoords = Vector2(1.f, 0.f);
			vertexes[vertexIndex3].texCoords = Vector2(0.f, 0.f);

			break;
		}
		default:
			break;
		}
	}

	/*if (numOfVerticsPerFace == 4)
	{
		for (int offset = 0; offset < vertexes.size(); offset += 4)
		{
			Vertex3 temp;
			temp = vertexes[offset + 0];
			vertexes[offset + 0] = vertexes[offset + 3];
			vertexes[offset + 3] = temp;
			temp = vertexes[offset + 1];
			vertexes[offset + 1] = vertexes[offset + 2];
			vertexes[offset + 2] = temp;
		}
	}*/


	return new VoxelModel(vertexes);
}

VoxelModel * VoxelModel::LoadFromQubicleBinary(const std::vector<unsigned char>& buffer)
{
	UINT32 version;
	UINT32 colorFormat;
	UINT32 zAxisOrientation;
	UINT32 compressed;
	UINT32 visibilityMaskEncoded;
	UINT32 numMatrices;
	//var f : file;
	UINT32 x;
	UINT32 y;
	UINT32 z;
	UINT32 sizeX;
	UINT32 sizeY;
	UINT32 sizeZ;
	INT32 posX;
	INT32 posY;
	INT32 posZ;
	std::vector<INT32> matrix;
	std::vector<std::vector<INT32>> matrixList;
	UINT32 index;
	UINT32 data;
	UINT32 count;
	const INT32 CODEFLAG = 2;
	const INT32 NEXTSLICEFLAG = 6;

	//f = fopen(filename);

	size_t bufferOffset = 0;

	version = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); // fread_uint32(f);
	colorFormat = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(f);
	zAxisOrientation = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(f);
	compressed = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(f);
	visibilityMaskEncoded = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(f);
	numMatrices = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(f);
	//matrixList = new array();

	for (UINT32 i = 0; i < numMatrices; i++) // for each matrix stored in file
	{
		// read matrix name
		BYTE nameLength = Persistence::ReadBuffer<BYTE>(buffer, bufferOffset);//fread_byte(file);
		std::string name = Persistence::ReadBufferAsString(buffer, bufferOffset, nameLength); //fread_string(file, nameLength);

		// read matrix size 
		sizeX = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);
		sizeY = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);
		sizeZ = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);

		// read matrix position (in this example the position is irrelevant)
		posX = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_int32(file);
		posY = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_int32(file);
		posZ = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_int32(file);

		// create matrix and add to matrix list
		matrix.resize(sizeX*sizeY*sizeZ); // = new array(sizeX*sizeY*sizeZ);
		matrixList.push_back(matrix);

		if (compressed == 0) // if uncompressd
		{
			for (z = 0; z < sizeZ; z++)
				for (y = 0; y < sizeY; y++)
					for (x = 0; x < sizeX; x++)
						matrix[x + y*sizeX + z*sizeX*sizeY] = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);
		}
		else // if compressed
		{
			z = 0;

			while (z < sizeZ)
			{
				z++;
				index = 0;

				while (true)
				{
					data = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);

					if (data == NEXTSLICEFLAG)
						break;
					else if (data == CODEFLAG)
					{
						count = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);
						data = Persistence::ReadBuffer<UINT32>(buffer, bufferOffset); //fread_uint32(file);

						for (UINT32 j = 0; j < count; j++)
						{
							x = index % (sizeX + 1);//index mod sizeX + 1; // mod = modulo e.g. 12 mod 8 = 4
							y = index / (sizeX + 1);//index div sizeX + 1; // div = integer division e.g. 12 div 8 = 1
							index++;
							matrix[x + y*sizeX + z*sizeX*sizeY] = data;
						}
					}
					else
					{
						x = index % (sizeX + 1); //index mod sizex + 1;
						y = index / (sizeX + 1); //index div sizex + 1;
						index++;
						matrix[x + y*sizeX + z*sizeX*sizeY] = data;
					}
				}
			}
		}
	}
	return nullptr;
}
