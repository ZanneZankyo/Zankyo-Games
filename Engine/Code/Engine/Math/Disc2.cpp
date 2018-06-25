#include "Engine/Math/Disc2.hpp"
#include "Vector2.hpp"
#include "MathUtils.hpp"

//const Disc2 Disc2::UNIT_CIRCLE(Vector2::ZERO, 1.f);
const Disc2 Disc2::UNIT_CIRCLE(Vector2(0.f,0.f), 1.f);

Disc2::Disc2()
	: position(Vector2(0.f, 0.f))
	, radius(0.f)
{}

Disc2::Disc2(const Disc2 & copy)
	: position(copy.position)
	, radius(copy.radius)
{}

Disc2::Disc2(float initialX, float initialY, float initialRadius)
	: position(initialX,initialY)
	, radius(initialRadius)
{}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius)
	: position(initialCenter)
	, radius(initialRadius)
{}

void Disc2::StretchToIncludePoint(const Vector2 & point)
{
	if (IsPointInside(point))
		return;
	float distance = CalcDistance(position, point);
	radius = distance;
}

void Disc2::AddPadding(float paddingRadius)
{
	radius += paddingRadius;
}

void Disc2::Translate(float x, float y)
{
	position += Vector2(x, y);
}

void Disc2::Translate(const Vector2 & translation)
{
	position += translation;
}

bool Disc2::IsPointInside(const Vector2 & point) const
{
	float distanceSquared = CalcDistanceSquared(position, point);
	return distanceSquared < radius * radius;
}

const Disc2 Disc2::operator+(const Vector2 & translation) const
{
	return Disc2(position+translation, radius);
}

const Disc2 Disc2::operator-(const Vector2 & antiTranslation) const
{
	return Disc2(position - antiTranslation, radius);
}

void Disc2::operator+=(const Vector2 & translation)
{
	position += translation;
}

void Disc2::operator-=(const Vector2 & antiTranslation)
{
	position -= antiTranslation;
}

bool DoDisc2sOverlap(const Disc2 & a, const Disc2 & b)
{
	float distanceSquared = CalcDistanceSquared(a.position, b.position);
	float sumOfRadius = a.radius + b.radius;
	return distanceSquared < sumOfRadius * sumOfRadius;
}

bool DoDisc2sOverlap(const Vector2 & aCenter, float aRadius, const Vector2 & bCenter, float bRadius)
{
	float distanceSquared = CalcDistanceSquared(aCenter, bCenter);
	float sumOfRadius = aRadius + bRadius;
	return distanceSquared < sumOfRadius * sumOfRadius;
}

const Disc2 Interpolate(const Disc2 & start, const Disc2 & end, float fractionToEnd)
{
	float interpolateRadius = start.radius * (1.f - fractionToEnd) + end.radius * fractionToEnd;
	return Disc2(Interpolate(start.position, end.position, fractionToEnd), interpolateRadius);
}



