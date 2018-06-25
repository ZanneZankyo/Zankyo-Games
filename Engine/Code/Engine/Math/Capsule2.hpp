#pragma once
#include "Ray2.hpp"

class Capsule2
{
public:
	Ray2 segment;
	float radius;

	Capsule2();
	Capsule2(const Capsule2& copy);
	explicit Capsule2(const Ray2& aSegment, float aRadius);
	explicit Capsule2(const Vector2& start, const Vector2& end, float aRadius);

	const Capsule2& operator = (const Capsule2& copy);
	bool operator == (const Capsule2& capsuleToEqual);
	bool operator != (const Capsule2& capsuleNotToEqual);
	friend const Capsule2 Interpolate(const Capsule2& start, const Capsule2& end, float fractionToEnd);
};