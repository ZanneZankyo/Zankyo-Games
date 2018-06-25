#include "Cone2.hpp"
#include "MathUtils.hpp"

Cone2::Cone2()
	: position()
	, startDirection()
	, endDirection()
	, outerRadius(0.f)
	, innerRadius(0.f)
	, degree(0.f)
{}

Cone2::Cone2(const Vector2& pos, const Vector2& start, const Vector2& end, float outerR, float innerR)
	: position(pos)
	, startDirection(start)
	, endDirection(end)
	, outerRadius(outerR)
	, innerRadius(innerR)
	, degree(0.f)
{
	float startDegree = start.CalcHeadingDegrees();
	float endDegree = end.CalcHeadingDegrees();
	degree = CalcShortestAngularDistance(startDegree, endDegree);
}

Cone2::Cone2(const Cone2& copy)
	: position(copy.position)
	, startDirection(copy.startDirection)
	, endDirection(copy.endDirection)
	, outerRadius(copy.outerRadius)
	, innerRadius(copy.innerRadius)
	, degree(copy.degree)
{}

bool Cone2::IsPointInside(const Vector2& pos) const
{
	float distanceSquared = CalcDistanceSquared(pos, position);
	if (distanceSquared > outerRadius * outerRadius || distanceSquared < innerRadius * innerRadius)
		return false;
	Vector2 direction = pos - position;
	Vector2 startCheck = startDirection;
	startCheck.Rotate90Degrees();
	Vector2 endCheck = endDirection;
	endCheck.RotateNegative90Degrees();

	float dotStart = DotProduct(startCheck, direction);
	float dotEnd = DotProduct(endCheck, direction);

	if (fabs(degree) < 180.f)
		return (dotStart >= 0.f && dotEnd >= 0.f);
	else
		return !(dotStart <= 0.f && dotEnd <= 0.f);
}

bool Cone2::IsAABB2Inside(const AABB2& bound) const
{
	Vector2 delta = bound.maxs - bound.mins;
	Vector2 checkVectors[4] =
	{
		bound.mins,
		bound.maxs,
		bound.mins + Vector2(0.f, delta.y),
		bound.mins + Vector2(delta.x, 0.f),
	};
	for (int index = 0; index < 4; index++)
	{
		if (!IsPointInside(checkVectors[index]))
			return false;
	}
	return true;
}
