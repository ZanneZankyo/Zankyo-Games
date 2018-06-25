#pragma once
#include <vector>
#include "Engine\Math\Vertex3.hpp"
#include "Engine\Math\AABB3.hpp"
#include "Engine\Math\AABB2.hpp"

class ShapeBuilder
{
public:
	static std::vector<Vertex3> Cube(const AABB3& cube, const Rgba& color = Rgba::WHITE, const AABB2& texCoords = AABB2::ZERO_TO_ONE);
	static std::vector<Vertex3> Cylinder(const Vector3& bottomPos, float height, float radius, int sides = 16, const Rgba& color = Rgba::WHITE);
	static std::vector<Vertex3> Sphere(const Vector3& center, float radius, int sides = 16, const Rgba& color = Rgba::WHITE);
};