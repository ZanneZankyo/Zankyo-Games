#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB3 AABB3::ZERO_TO_ONE(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f));
const AABB3 AABB3::NEG_ONE_TO_ONE(Vector3(-1.f, -1.f, -1.f), Vector3(1.f, 1.f, 1.f));

AABB3::AABB3()
	: mins(0.f,0.f,0.f)
	, maxs(0.f,0.f,0.f)
{}

AABB3::AABB3(const AABB3 & copy)
	: mins(copy.mins)
	, maxs(copy.maxs)
{}

AABB3::AABB3(float initialX, float initialY, float initialZ)
	: mins(0.f, 0.f,0.f)
	, maxs(initialX, initialY, initialZ)
{}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	: mins(minX,minY,minZ)
	, maxs(maxX,maxY,maxZ)
{}

AABB3::AABB3(const Vector3 & p_mins, const Vector3 & p_maxs)
	: mins(p_mins)
	, maxs(p_maxs)
{}

AABB3::AABB3(const Vector3 & center, float radiusX, float radiusY, float radiusZ)
	: mins(center - Vector3(radiusX, radiusY, radiusZ))
	, maxs(center + Vector3(radiusX, radiusY, radiusZ))
{}

AABB3::~AABB3()
{
	mins = Vector3::ZERO;
	maxs = Vector3::ZERO;
}

void AABB3::StretchToIncludePoint(const Vector3 & point)
{
	if (IsPointInside(point))
		return;

	if (point.x < mins.x)
		mins.x = point.x;
	if (point.y < mins.y)
		mins.y = point.y;
	if (point.z < mins.z)
		mins.z = point.z;
	if (maxs.x < point.x)
		maxs.x = point.x;
	if (maxs.y < point.y)
		maxs.y = point.y;
	if (maxs.z < point.z)
		maxs.z = point.z;
}

void AABB3::AddPaddingToSides(float xPaddingRadius, float yPaddingRadius, float zPaddingRadius)
{
	Vector3 delta(xPaddingRadius, yPaddingRadius, zPaddingRadius);
	mins -= delta;
	maxs += delta;
}

void AABB3::Translate(const Vector3 & translation)
{
	mins += translation;
	maxs += translation;
}

bool AABB3::IsPointInside(const Vector3 & point) const
{
	return (mins.x < point.x && point.x < maxs.x) && (mins.y < point.y && point.y < maxs.y) && (mins.z < point.z && point.z < maxs.z);
}

const Vector3 AABB3::CalcSize() const
{
	float sizeX = maxs.x - mins.x;
	float sizeY = maxs.y - mins.y;
	float sizeZ = maxs.z - mins.z;
	return Vector3(sizeX, sizeY, sizeZ);
}

const Vector3 AABB3::CalcCenter() const
{
	float centerX = (mins.x + mins.x) * 0.5f;
	float centerY = (mins.y + mins.y) * 0.5f;
	float centerZ = (mins.z + mins.z) * 0.5f;
	return Vector3(centerX, centerY, centerZ);
}

const AABB3 AABB3::operator+(const Vector3 & translation) const
{
	return AABB3(mins + translation, maxs + translation);
}

const AABB3 AABB3::operator-(const Vector3 & antiTranslation) const
{
	return AABB3(mins - antiTranslation, maxs - antiTranslation);
}

void AABB3::operator+=(const Vector3 & translation)
{
	mins += translation;
	maxs += translation;
}

void AABB3::operator-=(const Vector3 & antiTranslation)
{
	mins -= antiTranslation;
	maxs -= antiTranslation;
}

bool DoAABB3sOverlap(const AABB3 & a, const AABB3 & b)
{
	if (a.mins.y > b.maxs.y)
		return false;
	if (a.mins.x > b.maxs.x)
		return false;
	if (a.mins.z > b.maxs.z)
		return false;
	if (a.maxs.x < b.mins.x)
		return false;
	if (a.maxs.y < b.mins.y)
		return false;
	if (a.maxs.z < b.mins.z)
		return false;
	return true;
}

const AABB3 Interpolate(const AABB3 & start, const AABB3 & end, float fractionToEnd)
{
	return AABB3(Interpolate(start.mins, end.mins, fractionToEnd), Interpolate(start.maxs, end.maxs, fractionToEnd));
}
