#include "Engine/Math/Sphere3.hpp"
#include "Vector3.hpp"
#include "MathUtils.hpp"

//const Sphere3 Sphere3::UNIT_CIRCLE(Vector3::ZERO, 1.f);
const Sphere3 Sphere3::UNIT_SPHERE(Vector3(0.f,0.f,0.f), 1.f);

bool DoSphere3sOverlap(const Sphere3 & a, const Sphere3 & b)
{
	float sumOfRadius = a.radius + b.radius;
	return CalcDistanceSquared(a.position,b.position) < sumOfRadius * sumOfRadius;
}

const Sphere3 Interpolate(const Sphere3 & start, const Sphere3 & end, float fractionToEnd)
{
	float interpolateRadius = start.radius * (1.f - fractionToEnd) + end.radius * fractionToEnd;
	return Sphere3(Interpolate(start.position, end.position, fractionToEnd), interpolateRadius);
}
