#pragma once
#include <vector>
#include "Engine\Math\Vertex3.hpp"
#include "Engine\Math\AABB3.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Mesh.hpp"
#include "Engine\RHI\Font.hpp"
#include "Skeleton.hpp"
#include "Motion.hpp"
#include "Engine\Math\MatrixStack.hpp"
#include "BlendShape.hpp"

class D3D11Renderer;

class MeshBuilder
{
public:
	MeshBuilder();
	~MeshBuilder();
	std::vector<Vertex3> m_vertices;
	std::vector<uint32_t> m_indices;
	std::vector<DrawInstruction> m_drawInstructions;
	bool m_useIndex;
	MeshBuilder* m_parent;
	std::string m_name;
	std::string m_materialName;
	std::vector<MeshBuilder*> m_children;
	PrimitiveType m_primitiveType;
	BlendShape m_blendShape;

	void FlipX();
	void Begin(PrimitiveType primitiveType, bool useIndex);
	void Begin(const std::string& materialName, PrimitiveType primitiveType, bool useIndex);
	void End();
	bool HasIndex(uint32_t index);
	void AddVertex(const Vertex3& vertex);
	void AddIndexedVertex(const Vertex3& vertex, uint32_t index);
	void Clear();
public:
	static std::vector<Vertex3> Cube(const AABB3& cube, const Rgba& color = Rgba::WHITE, const AABB2& texCoords = AABB2::ZERO_TO_ONE);
	static std::vector<Vertex3> Cylinder(const Vector3& bottomPos, float height, float radius, int sides = 16, const Rgba& color = Rgba::WHITE);
	//Sphere: return quads
	static std::vector<Vertex3> Sphere(const Vector3& center, float radius, int sides = 16, const Rgba& color = Rgba::WHITE);
	static std::vector<Vertex3> Squard(const AABB2& bound, const Rgba& color = Rgba::WHITE, const AABB2& texCoords = AABB2::ZERO_TO_ONE, float depth = 0.f);
	static std::vector<Vertex3> Text2D(Vector2& pos, float scale, Rgba color, Font* font, std::string string
		, TextAlignHorizontal alignHorizontal = TEXTALIGN_HORIZONTAL_LEFT
		, TextAlignVertical alignVertical = TEXTALIGN_VERTICAL_TOP
		, int cursorIndex = -1);
	static std::vector<Vertex3> Disc2D(const Vector2& center, float radius, int sides = 16, const Rgba& color = Rgba::WHITE);
	static std::vector<Vertex3> SkeletonLines(Skeleton* skeleton);
	static std::vector<Vertex3> SkeletonIns(SkeletonInstance* skeleton);
private:
	static void AddSkeletonJoint(std::vector<Vertex3>& vertices, MatrixStack& mat, Skeleton* skeleton, SkeletonJoint* joint);
	static void AddSkeletonJointPose(std::vector<Vertex3>& vertices, MatrixStack& mat, Skeleton* skeleton, SkeletonJoint* joint, Pose* pose = nullptr);
public:
	Mesh* OutputMeshData() const;
	void OutputMeshesData(std::vector<Mesh*>* meshes) const;
};