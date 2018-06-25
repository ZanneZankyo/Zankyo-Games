#include "ShapeBuilder.hpp"
#include "Engine\Math\MathUtils.hpp"

std::vector<Vertex3> ShapeBuilder::Cube(const AABB3& cube, const Rgba& color /*= Rgba::WHITE*/, const AABB2& texCoords /*= AABB2::ONE*/)
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

std::vector<Vertex3> ShapeBuilder::Cylinder(const Vector3& bottomPos, float height, float radius, int sides /*= 16*/, const Rgba& color /*= Rgba::WHITE*/)
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

std::vector<Vertex3> ShapeBuilder::Sphere(const Vector3& center, float radius, int sides /*= 16*/, const Rgba& color /*= Rgba::WHITE*/)
{
	float deltaDegree = 360.f / sides;
	float ration = 1.f / sides;
	std::vector<Vertex3> vertices;
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

