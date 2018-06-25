#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB2 AABB2::ZERO_TO_ONE(Vector2(0.f, 0.f), Vector2(1.f, 1.f));
const AABB2 AABB2::NEG_ONE_TO_ONE(Vector2(-1.f, -1.f), Vector2(1.f, 1.f));

AABB2::AABB2()
	: mins(0.f,0.f)
	, maxs(0.f,0.f)
{}

AABB2::AABB2(const AABB2 & copy)
	: mins(copy.mins)
	, maxs(copy.maxs)
{}

AABB2::AABB2(float initialX, float initialY)
	: mins(0.f, 0.f)
	, maxs(initialX, initialY)
{}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	: mins(minX,minY)
	, maxs(maxX,maxY)
{}

AABB2::AABB2(const Vector2 & p_mins, const Vector2 & p_maxs)
	: mins(p_mins)
	, maxs(p_maxs)
{}

AABB2::AABB2(const Vector2 & center, float radiusX, float radiusY)
	: mins(center - Vector2(radiusX, radiusY))
	, maxs(center + Vector2(radiusX, radiusY))
{}

AABB2::~AABB2()
{
	mins = Vector2::ZERO;
	maxs = Vector2::ZERO;
}

void AABB2::StretchToIncludePoint(const Vector2 & point)
{
	if (IsPointInside(point))
		return;

	if(point.x < mins.x)
		mins.x = point.x;
	if(point.y < mins.y)
		mins.y = point.y;
	if(maxs.x < point.x)
		maxs.x = point.x;
	if(maxs.y < point.y)
		maxs.y = point.y;

}

void AABB2::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius)
{
	Vector2 delta(xPaddingRadius, yPaddingRadius);
	mins -= delta;
	maxs += delta;
}

void AABB2::Translate(const Vector2 & translation)
{
	mins += translation;
	maxs += translation;
}

bool AABB2::IsPointInside(const Vector2 & point) const
{
	return (mins.x < point.x && point.x < maxs.x) && (mins.y < point.y && point.y < maxs.y);
}

const Vector2 AABB2::CalcSize() const
{
	float sizeX = maxs.x - mins.x;
	float sizeY = maxs.y - mins.y;
	return Vector2(sizeX, sizeY);
}

const Vector2 AABB2::CalcCenter() const
{
	float centerX = (mins.x + mins.x) * 0.5f;
	float centerY = (mins.y + mins.y) * 0.5f;
	return Vector2(centerX, centerY);
}

Vector2 AABB2::GetUV(const Vector2& position) const
{
	float x = RangeMap(position.x, mins.x, maxs.x, 0.f, 1.f);
	float y = RangeMap(position.y, mins.y, maxs.y, 0.f, 1.f);
	return Vector2(x, y);
}

Vector2 AABB2::EvaluateFromUV(const Vector2 & uv) const
{
	float x = RangeMap(uv.x, 0.f, 1.f, mins.x, maxs.x);
	float y = RangeMap(uv.y, 0.f, 1.f, mins.y, maxs.y);
	return Vector2(x, y);
}

const AABB2 AABB2::operator+(const Vector2 & translation) const
{
	return AABB2(mins + translation, maxs + translation);
}

const AABB2 AABB2::operator-(const Vector2 & antiTranslation) const
{
	return AABB2(mins - antiTranslation, maxs - antiTranslation);
}

void AABB2::operator+=(const Vector2 & translation)
{
	mins += translation;
	maxs += translation;
}

void AABB2::operator-=(const Vector2 & antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

bool AABB2::operator==(const AABB2& boundsToCompare) const
{
	return mins == boundsToCompare.mins && maxs == boundsToCompare.maxs;
}

bool AABB2::operator!=(const AABB2& boundsToCompare) const
{
	return !(*this == boundsToCompare);
}

AABB2 AABB2::CreateFromCenterAndSize(const Vector2 & center, const Vector2 & size)
{
	return AABB2(center - size*0.5f, center + size*0.5f);
}

bool DoAABB2sOverlap(const AABB2 & a, const AABB2 & b)
{
	if (a.mins.y > b.maxs.y)
		return false;
	if (a.mins.x > b.maxs.x)
		return false;
	if (a.maxs.x < b.mins.x)
		return false;
	if (a.maxs.y < b.mins.y)
		return false;
	return true;
}

const AABB2 Interpolate(const AABB2 & start, const AABB2 & end, float fractionToEnd)
{
	return AABB2(Interpolate(start.mins, end.mins, fractionToEnd), Interpolate(start.maxs, end.maxs, fractionToEnd));
}
