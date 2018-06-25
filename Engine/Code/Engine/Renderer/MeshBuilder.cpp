#include "MeshBuilder.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "D3D11Renderer.hpp"
#include "Engine\RHI\Font.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Math\MatrixStack.hpp"
#include "Skeleton.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Core\Profiler.hpp"
#include "..\Core\StringUtils.hpp"

MeshBuilder::MeshBuilder()
	: m_vertices()
	, m_indices()
	, m_drawInstructions()
	, m_useIndex(true)
	, m_parent(nullptr)
	, m_children()
	, m_blendShape()
{
}

MeshBuilder::~MeshBuilder()
{
	for (auto child: m_children)
		delete child;
}

void MeshBuilder::FlipX()
{
	if (!m_vertices.empty())
		for (auto& vertex : m_vertices)
		{
			vertex.position.x *= -1.f;
			vertex.normal.x *= -1.f;
			//vertex.texCoords.x = 1.f - vertex.texCoords.x;
			vertex.texCoords.y = 1.f - vertex.texCoords.y;
		}
	for (auto child : m_children)
		child->FlipX();
}

void MeshBuilder::Begin(PrimitiveType primitiveType, bool useIndex)
{
	m_primitiveType = primitiveType;
	m_useIndex = useIndex;
}

void MeshBuilder::Begin(const std::string& materialName, PrimitiveType primitiveType, bool useIndex)
{
	m_materialName = materialName;
	m_primitiveType = primitiveType;
	m_useIndex = useIndex;
}

void MeshBuilder::End()
{
	
}

bool MeshBuilder::HasIndex(uint32_t index)
{
	if (!m_useIndex)
		return false;
	return find(m_indices.begin(), m_indices.end(), index) != m_indices.end();
}

void MeshBuilder::AddVertex(const Vertex3& vertex)
{
	m_vertices.push_back(vertex);
}

void MeshBuilder::AddIndexedVertex(const Vertex3& vertex, uint32_t index)
{
	if(m_vertices.size() <= index)
		m_vertices.resize(index + 1);
	m_vertices[index] = vertex;
	m_indices.push_back(index);
}

void MeshBuilder::Clear()
{
	m_vertices.clear();
	m_indices.clear();
}

Mesh* MeshBuilder::OutputMeshData() const
{
	return new Mesh(m_primitiveType, m_name, m_vertices, nullptr, nullptr, m_useIndex, m_indices, nullptr);
}

void MeshBuilder::OutputMeshesData(std::vector<Mesh*>* meshes) const
{
	if (!m_vertices.empty())
		meshes->push_back(OutputMeshData());
	for (auto child : m_children)
		child->OutputMeshesData(meshes);
}

std::vector<Vertex3> MeshBuilder::Cube(const AABB3& cube, const Rgba& color /*= Rgba::WHITE*/, const AABB2& texCoords /*= AABB2::ONE*/)
{

	Vector2 texTopLeft(texCoords.mins);
	Vector2 texBottomRight(texCoords.maxs);
	Vector2 texBottomLeft(texCoords.mins.x, texCoords.maxs.y);
	Vector2 texTopRight(texCoords.maxs.x, texCoords.mins.y);

	Vertex3 vertices[] = {
		//near
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.mins.z), color, texBottomLeft),
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.mins.z), color, texBottomRight),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.mins.z), color, texTopRight),
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.mins.z), color, texTopLeft),

		//right
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.mins.z), color, texBottomLeft),
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.maxs.z), color, texBottomRight),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.maxs.z), color, texTopRight),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.mins.z), color, texTopLeft),

		//far
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.maxs.z), color, texBottomLeft),
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.maxs.z), color, texBottomRight),
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.maxs.z), color, texTopRight),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.maxs.z), color, texTopLeft),

		//left
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.maxs.z), color, texBottomLeft),
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.mins.z), color, texBottomRight),
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.mins.z), color, texTopRight),
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.maxs.z), color, texTopLeft),

		//top
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.mins.z), color, texBottomLeft),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.mins.z), color, texBottomRight),
		Vertex3(Vector3(cube.maxs.x,	cube.maxs.y,	cube.maxs.z), color, texTopRight),
		Vertex3(Vector3(cube.mins.x,	cube.maxs.y,	cube.maxs.z), color, texTopLeft),

		//bottom
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.mins.z), color, texBottomLeft),
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.mins.z), color, texBottomRight),
		Vertex3(Vector3(cube.mins.x,	cube.mins.y,	cube.maxs.z), color, texTopRight),
		Vertex3(Vector3(cube.maxs.x,	cube.mins.y,	cube.maxs.z), color, texTopLeft),
	};
	int numOfVertices = sizeof(vertices) / sizeof(Vertex3);
	return std::vector<Vertex3>(vertices, vertices + numOfVertices);
}

std::vector<Vertex3> MeshBuilder::Cylinder(const Vector3& bottomPos, float height, float radius, int sides /*= 16*/, const Rgba& color /*= Rgba::WHITE*/)
{
	float deltaDegree = 360.f / sides;
	float ration = 1.f / sides;
	std::vector<Vertex3> vertices;
	vertices.reserve(sides * 12);
	Vector3 topPos(bottomPos);
	Vector2 centerTexCoords(0.5f, 0.5f);
	topPos.y += height;
	for (int sideIndex = 0; sideIndex < sides; sideIndex++)
	{

		float currentDegree = sideIndex * deltaDegree;
		float nextDegree = currentDegree + deltaDegree;

		Vector2 currentDirection2(CosDegrees(currentDegree), SinDegrees(currentDegree));
		Vector2 nextDirection2(CosDegrees(nextDegree), SinDegrees(nextDegree));
		Vector3 currentDirection3(currentDirection2.x, 0.f, currentDirection2.y);
		Vector3 nextDirection3(nextDirection2.x, 0.f, nextDirection2.y);
		Vector3 currentOffset3(currentDirection3 * radius);
		Vector3 nextOffset3(nextDirection3 * radius);
		Vector3 currentTangent3 = -CrossProduct(currentDirection3, Vector3(0.f, -1.f, 0.f));
		Vector3 nextTangent3 = -CrossProduct(nextDirection3, Vector3(0.f, -1.f, 0.f));

		//bottom
		vertices.push_back(Vertex3(bottomPos,					color, centerTexCoords,								Vector3(0.f, -1.f, 0.f), Vector3(1.f,0.f,0.f)));
		vertices.push_back(Vertex3(bottomPos + nextOffset3,		color, centerTexCoords - nextDirection2 * 0.5f,		Vector3(0.f, -1.f, 0.f), Vector3(1.f,0.f,0.f)));
		vertices.push_back(Vertex3(bottomPos + currentOffset3,	color, centerTexCoords - currentDirection2 * 0.5f,	Vector3(0.f, -1.f, 0.f), Vector3(1.f,0.f,0.f)));
		vertices.push_back(Vertex3(bottomPos,					color, centerTexCoords,								Vector3(0.f, -1.f, 0.f), Vector3(1.f,0.f,0.f)));

		//sides
		AABB2 sideTexCoords(Vector2::ZERO, Vector2(ration, 1.f));
		sideTexCoords += Vector2(ration * sideIndex, 0.f);
		
		vertices.push_back(Vertex3(bottomPos + currentOffset3,	color, Vector2(sideTexCoords.mins.x, sideTexCoords.maxs.y), currentDirection3, currentTangent3));
		vertices.push_back(Vertex3(bottomPos + nextOffset3,		color, Vector2(sideTexCoords.maxs.x, sideTexCoords.maxs.y), nextDirection3, nextTangent3));
		vertices.push_back(Vertex3(topPos + nextOffset3,		color, Vector2(sideTexCoords.maxs.x, sideTexCoords.mins.y), nextDirection3, nextTangent3));
		vertices.push_back(Vertex3(topPos + currentOffset3,		color, Vector2(sideTexCoords.mins.x, sideTexCoords.mins.y), currentDirection3, currentTangent3));

		//Vector2 topTexCoords[4]
		//top
		vertices.push_back(Vertex3(topPos, color, centerTexCoords, Vector3(0.f, 1.f, 0.f), Vector3(-1.f, 0.f, 0.f)));
		vertices.push_back(Vertex3(topPos + currentOffset3, color, centerTexCoords - currentDirection2 * 0.5f, Vector3(0.f, 1.f, 0.f), Vector3(-1.f, 0.f, 0.f)));
		vertices.push_back(Vertex3(topPos + nextOffset3, color, centerTexCoords - nextDirection2 * 0.5f, Vector3(0.f, 1.f, 0.f), Vector3(-1.f, 0.f, 0.f)));
		vertices.push_back(Vertex3(topPos, color, centerTexCoords, Vector3(0.f, 1.f, 0.f), Vector3(-1.f, 0.f, 0.f)));
	}
	return vertices;
}

std::vector<Vertex3> MeshBuilder::Sphere(const Vector3& center, float radius, int sides /*= 16*/, const Rgba& color /*= Rgba::WHITE*/)
{
	PROFILE_FUNCTION;
	float deltaDegree = 360.f / sides;
	float ration = 1.f / sides;
	std::vector<Vertex3> vertices;
	vertices.reserve(sides * sides * sides * 4);
	for (int longitudeIndex = 0; longitudeIndex < sides; longitudeIndex++)
	{
		float currentLongituteDegrees = longitudeIndex * deltaDegree;
		float nextLongituteDegrees = currentLongituteDegrees + deltaDegree;
		float currentLongituteRation = ration * longitudeIndex;
		float nextLongituteRation = currentLongituteRation + ration;
		for (int latitudeIndex = 0; latitudeIndex < sides / 2; latitudeIndex++) //from top to down
		{
			float currentLatituteDegrees = latitudeIndex * deltaDegree;
			float nextLatituteDegrees = currentLatituteDegrees + deltaDegree;
			float currentLatituteRation = ration * latitudeIndex * 2.f;
			float nextLatituteRation = currentLatituteRation + ration * 2.f;

			

			// current longitude current latitude
			float currentLatituteSinDegrees = SinDegrees(currentLatituteDegrees);
			Vector3 cLoCLaDirection(CosDegrees(currentLongituteDegrees) * currentLatituteSinDegrees
				, CosDegrees(currentLatituteDegrees)
				, SinDegrees(currentLongituteDegrees) * currentLatituteSinDegrees);
			Vector3 cLoCLaTangent = CrossProduct(Vector3(0.f, 1.f, 0.f),cLoCLaDirection);
			
			// current longitude next latitude
			float nextLatituteSinDegrees = SinDegrees(nextLatituteDegrees);
			Vector3 cLoNLaDirection(CosDegrees(currentLongituteDegrees) * nextLatituteSinDegrees
				, CosDegrees(nextLatituteDegrees)
				, SinDegrees(currentLongituteDegrees) * nextLatituteSinDegrees);
			Vector3 cLoNLaTangent = CrossProduct(Vector3(0.f, 1.f, 0.f), cLoNLaDirection);
			// next longitude current latitude
			Vector3 nLoCLaDirection(CosDegrees(nextLongituteDegrees) * currentLatituteSinDegrees
				, CosDegrees(currentLatituteDegrees)
				, SinDegrees(nextLongituteDegrees) * currentLatituteSinDegrees);
			Vector3 nLoCLaTangent = CrossProduct(Vector3(0.f, 1.f, 0.f), nLoCLaDirection);
			// next longitude next latitude
			Vector3 nLoNLaDirection(CosDegrees(nextLongituteDegrees) * nextLatituteSinDegrees
				, CosDegrees(nextLatituteDegrees)
				, SinDegrees(nextLongituteDegrees) * nextLatituteSinDegrees);
			Vector3 nLoNLaTangent = CrossProduct(Vector3(0.f, 1.f, 0.f), nLoNLaDirection);

			vertices.push_back(Vertex3(center + cLoNLaDirection * radius, color, Vector2(currentLongituteRation, nextLatituteRation), cLoNLaDirection, cLoNLaTangent));
			vertices.push_back(Vertex3(center + nLoNLaDirection * radius, color, Vector2(nextLongituteRation, nextLatituteRation), nLoNLaDirection, nLoNLaTangent));
			vertices.push_back(Vertex3(center + nLoCLaDirection * radius, color, Vector2(nextLongituteRation, currentLatituteRation), nLoCLaDirection, nLoCLaTangent));
			vertices.push_back(Vertex3(center + cLoCLaDirection * radius, color, Vector2(currentLongituteRation, currentLatituteRation), cLoCLaDirection, cLoCLaTangent));
				
// 			vertices.push_back(Vertex3(center + cLoCLaDirection * radius, color, Vector2(0.f, 0.f), cLoCLaDirection));
// 			vertices.push_back(Vertex3(center + cLoNLaDirection * radius, color, Vector2(0.f, 1.f), cLoNLaDirection));
// 			vertices.push_back(Vertex3(center + nLoNLaDirection * radius, color, Vector2(1.f, 1.f), nLoNLaDirection));
// 			vertices.push_back(Vertex3(center + nLoCLaDirection * radius, color, Vector2(1.f, 0.f), nLoCLaDirection));
		}
	}
	
	return vertices;
}

std::vector<Vertex3> MeshBuilder::Squard(const AABB2 & bound, const Rgba & color, const AABB2 & texCoords, float depth)
{
	std::vector<Vertex3> vertices;
	vertices.reserve(6);
	vertices.push_back(Vertex3(Vector3(bound.mins.x, bound.mins.y, depth), color, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	vertices.push_back(Vertex3(Vector3(bound.maxs.x, bound.mins.y, depth), color, Vector2(texCoords.maxs.x, texCoords.maxs.y)));
	vertices.push_back(Vertex3(Vector3(bound.maxs.x, bound.maxs.y, depth), color, Vector2(texCoords.maxs.x, texCoords.mins.y)));

	vertices.push_back(Vertex3(Vector3(bound.mins.x, bound.mins.y, depth), color, Vector2(texCoords.mins.x, texCoords.maxs.y)));
	vertices.push_back(Vertex3(Vector3(bound.maxs.x, bound.maxs.y, depth), color, Vector2(texCoords.maxs.x, texCoords.mins.y)));
	vertices.push_back(Vertex3(Vector3(bound.mins.x, bound.maxs.y, depth), color, Vector2(texCoords.mins.x, texCoords.mins.y)));
	return vertices;
}

std::vector<Vertex3> MeshBuilder::Text2D(Vector2& pos, float scale, Rgba color, Font* font, std::string string
	, TextAlignHorizontal alignHorizontal /*= TEXTALIGN_HORIZONTAL_LEFT */, TextAlignVertical alignVertical /*= TEXTALIGN_VERTICAL_TOP */
	, int cursorIndex /*= -1*/)
{
	UNUSED(cursorIndex);
	PROFILE_FUNCTION;
	std::vector<float> textWidthes = font->CalculateTextWidthes(string, scale);
	float textHeight = font->CalculateTextHeight(string, scale);
	size_t widthIndex = 0;

	Vector2 origin = pos;

	switch (alignHorizontal)
	{
	case TEXTALIGN_HORIZONTAL_LEFT:
		break;
	case TEXTALIGN_HORIZONTAL_CENTER:
		pos.x -= textWidthes[widthIndex] * 0.5f;
		break;
	case TEXTALIGN_HORIZONTAL_RIGHT:
		pos.x -= textWidthes[widthIndex];
		break;
	default:
		break;
	}

	switch (alignVertical)
	{
	case TEXTALIGN_VERTICAL_TOP:
		break;
	case TEXTALIGN_VERTICAL_CENTER:
		pos.y += textHeight * 0.5f;
		break;
	case TEXTALIGN_VERTICAL_BOTTOM:
		pos.y += textHeight - (font->m_lineHeight - font->m_base)*scale;
		break;
	default:
		break;
	}
	

	Vector2 currentPos = pos;
	Vector2 lineStartPos = pos;

	std::vector<Vertex3> vertices;
	vertices.reserve(string.length() * 6);

	for (size_t stringIndex = 0; stringIndex < string.size(); stringIndex++)
	{
		char printChar = string[stringIndex];

		if (printChar == '\n')
		{
			widthIndex++;
			lineStartPos.y -= font->m_lineHeight * scale;
			switch (alignHorizontal)
			{
			case TEXTALIGN_HORIZONTAL_LEFT:
				lineStartPos.x = origin.x;
				break;
			case TEXTALIGN_HORIZONTAL_CENTER:
				lineStartPos.x = origin.x - textWidthes[widthIndex] * 0.5f;
				break;
			case TEXTALIGN_HORIZONTAL_RIGHT:
				lineStartPos.x = origin.x - textWidthes[widthIndex];
				break;
			default:
				break;
			}
			currentPos = lineStartPos;
			continue;
		}

		//std::map<int, FontTag>::iterator tagFound = font->m_tags.find(printChar);

		//FontTag& tag = tagFound->second;

		//ASSERT_OR_DIE(tagFound != font->m_tags.end(), "undefined font tags of char " + printChar);
		//if (tagFound == font->m_tags.end())
		// 	continue;

		if (printChar >= (int)font->m_tags.size())
			continue;

		//FontTag tag = tagFound->second;
		FontTag& tag = font->m_tags[printChar];

		Vector2 charOffset(tag.xoffset, -tag.yoffset-tag.height);

		if (stringIndex > 0)
		{
			char prevChar = string[stringIndex];
			//auto kerningFound = font->m_kernings.find(std::pair<int, int>(prevChar, printChar));
			//if (kerningFound != font->m_kernings.end())
			//	charOffset.x += kerningFound->second;
			if(prevChar < (int)font->m_kernings.size() && printChar < (int)font->m_kernings[prevChar].size())
				charOffset.x += font->m_kernings[prevChar][printChar];
		}

		Vector2 drawPos = currentPos + charOffset * scale;
		Vector2 charSize(tag.width, tag.height);

		AABB2 quad(drawPos, drawPos + charSize * scale);

		AABB2 uv((float)tag.x, (float)tag.y, (float)(tag.x + tag.width), (float)(tag.y + tag.height));
		uv.mins.x /= font->m_scaleW;
		uv.maxs.x /= font->m_scaleW;
		uv.mins.y /= font->m_scaleH;
		uv.maxs.y /= font->m_scaleH;

		if (printChar != ' ' && printChar != '\n')
		{
			Vertex3 v0(Vector3(quad.mins.x, quad.mins.y, 0.f), color, Vector2(uv.mins.x, uv.maxs.y));
			Vertex3 v1(Vector3(quad.maxs.x, quad.mins.y, 0.f), color, Vector2(uv.maxs.x, uv.maxs.y));
			Vertex3 v2(Vector3(quad.maxs.x, quad.maxs.y, 0.f), color, Vector2(uv.maxs.x, uv.mins.y));
			Vertex3 v3(Vector3(quad.mins.x, quad.maxs.y, 0.f), color, Vector2(uv.mins.x, uv.mins.y));

			vertices.emplace_back(v0);
			vertices.emplace_back(v1);
			vertices.emplace_back(v2);
			vertices.emplace_back(v0);
			vertices.emplace_back(v2);
			vertices.emplace_back(v3);
		}
		
		currentPos.x += tag.xadvance * scale;
	}
	pos = currentPos;
	return vertices;
}

std::vector<Vertex3> MeshBuilder::Disc2D(const Vector2& center, float radius, int sides /*= 16*/, const Rgba& color /*= Rgba::WHITE*/)
{
	std::vector<Vertex3> vertices;
	vertices.reserve(sides * 3);
	float deltaDegrees = 360.f / sides;
	Vector2 prevPos = center + Vector2(radius,0.f);
	for (float degrees = deltaDegrees; degrees <= 360.f; degrees += deltaDegrees)
	{
		Vector2 currentPos = center + Vector2::GetNormalizeFormDegrees(degrees) * radius;
		vertices.emplace_back(center, color);
		vertices.emplace_back(prevPos, color);
		vertices.emplace_back(currentPos, color);
		prevPos = currentPos;
	}
	return vertices;
}

std::vector<Vertex3> MeshBuilder::SkeletonIns(SkeletonInstance* skeletonIns)
{
	MatrixStack mat;
	std::vector<Vertex3> vertices;
	int jointCount = skeletonIns->m_skeleton->GetJointCount();
	for (int jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		int parentIndex = skeletonIns->GetJointParent(jointIndex);
		
		if (parentIndex == -1) continue;
		Matrix4 current = skeletonIns->GetJointGlobalTransform(jointIndex);
		Matrix4 parent = skeletonIns->GetJointGlobalTransform(parentIndex);
		vertices.push_back(Vertex3(current.GetPosition()));
		vertices.push_back(Vertex3(parent.GetPosition()));
		
	}
	//AddSkeletonJointPose(vertices, mat, skeletonIns->m_skeleton, skeletonIns->m_skeleton->m_root, &skeletonIns->m_currentPose);
	return vertices;
}

std::vector<Vertex3> MeshBuilder::SkeletonLines(Skeleton* skeleton)
{
	MatrixStack mat;
	std::vector<Vertex3> vertices;
	//AddSkeletonJoint(vertices, mat, skeleton, skeleton->m_root);
	int jointCount = skeleton->GetJointCount();
	for (int jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		int parentIndex = skeleton->GetJointParentIndex(jointIndex);

		if (parentIndex == -1) continue;
		SkeletonJoint* currentJoint = skeleton->GetJoint(jointIndex);
		//SkeletonJoint* parentJoint = skeleton->GetJoint(parentIndex);
		Matrix4 current = currentJoint->m_transform;
		Matrix4 parent = skeleton->GetJoint(parentIndex)->m_transform;
		vertices.push_back(Vertex3(current.GetPosition()));
		vertices.push_back(Vertex3(parent.GetPosition()));
	}

	return vertices;
}

void MeshBuilder::AddSkeletonJoint(std::vector<Vertex3>& vertices, MatrixStack& mat, Skeleton* skeleton, SkeletonJoint* joint)
{
	if (!joint)
		return;
	//int jointIndex = skeleton->GetJointIndex(joint);
	Vector3 jointPosition;

		jointPosition = joint->m_transform.GetPosition();

	for (auto child : joint->m_children)
	{
		vertices.push_back(Vertex3(jointPosition));

		Vector3 childPosition;
		childPosition = child->m_transform.GetPosition();
		vertices.push_back(Vertex3(childPosition));
		AddSkeletonJoint(vertices, mat, skeleton, child);
	}
}

void MeshBuilder::AddSkeletonJointPose(std::vector<Vertex3>& vertices, MatrixStack& mat, Skeleton* skeleton, SkeletonJoint* joint, Pose* pose)
{
	if (!joint || !pose)
		return;
	
	Vector3 jointPosition;

	int jointIndex = skeleton->GetJointIndex(joint);
	//mat.Push(pose->GetTransforms(jointIndex));
	jointPosition = (joint->m_transform * pose->GetTransform(jointIndex)).GetPosition();//(mat.Top()).GetPosition();

	for (auto child : joint->m_children)
	{
		vertices.push_back(Vertex3(jointPosition));
		Vector3 childPosition;
		int childIndex = skeleton->GetJointIndex(child);
		Matrix4 localTramsform = child->m_transform * joint->m_transform.GetInverse();
		mat.Push(pose->GetTransform(childIndex));
		childPosition = (child->m_transform * pose->GetTransform(childIndex)).GetPosition();//(mat.Top()).GetPosition();
		mat.Pop();
		vertices.push_back(Vertex3(childPosition));
		AddSkeletonJointPose(vertices, mat, skeleton, child, pose);
		mat.Pop();
	}
}

