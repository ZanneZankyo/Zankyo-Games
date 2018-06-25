#pragma once
#include "Vector2.hpp"
#include "AABB2.hpp"


class Cone2
{
public:
	Vector2 position;
	Vector2 startDirection; // counter clockwise start direction
	Vector2 endDirection; // counter clockwise end direction
	float outerRadius;
	float innerRadius;
	float degree;

	Cone2();
	Cone2(const Vector2& pos, const Vector2& start, const Vector2& end, float outerR, float innerR = 0.f);
	Cone2(const Cone2& copy);

	bool IsPointInside(const Vector2& pos) const;
	bool IsAABB2Inside(const AABB2& bound) const;
};