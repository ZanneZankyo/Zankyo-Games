#include "Engine/Math/Capsule2.hpp"
#include "MathUtils.hpp"

Capsule2::Capsule2()
	: segment()
	, radius(0.f)
{
}

Capsule2::Capsule2(const Capsule2 & copy)
	: segment(copy.segment)
	, radius(copy.radius)
{
}

Capsule2::Capsule2(const Ray2 & aSegment, float aRadius)
	: segment(aSegment)
	, radius(aRadius)
{
}

Capsule2::Capsule2(const Vector2 & start, const Vector2 & end, float aRadius)
	: segment(start, end)
	, radius(aRadius)
{
}

const Capsule2 & Capsule2::operator=(const Capsule2 & copy)
{
	segment = copy.segment;
	radius = copy.radius;
	return *this;
}

bool Capsule2::operator==(const Capsule2 & capsuleToEqual)
{
	return segment == capsuleToEqual.segment && radius == capsuleToEqual.radius;
}

bool Capsule2::operator!=(const Capsule2 & capsuleNotToEqual)
{
	return !(*this == capsuleNotToEqual);
}

const Capsule2 Interpolate(const Capsule2 & start, const Capsule2 & end, float fractionToEnd)
{
	float interpolateRadius = start.radius * (1.f - fractionToEnd) + end.radius * fractionToEnd;
	return Capsule2(Interpolate(start.segment,end.segment,fractionToEnd), interpolateRadius);
}
