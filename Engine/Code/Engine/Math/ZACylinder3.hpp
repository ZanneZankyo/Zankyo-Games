#pragma once
#include "Vector3.hpp"

class ZACylinder
{
public:
	Vector3 centerPosition;
	float height;
	float radius;

	ZACylinder();
	ZACylinder(const ZACylinder& copy);
	explicit ZACylinder(float centerX, float centerY, float centerZ, float _height, float _radius);
	explicit ZACylinder(const Vector3& centerPos, float _height, float _radius);

	bool IsPointInside(const Vector3& pos) const;

	Vector3 GetBottomPosition() const;
	Vector3 GetTopPosition() const;

	friend bool DoZACylindersOverlap(const ZACylinder& a, const ZACylinder& b);
};

inline ZACylinder::ZACylinder()
	: centerPosition()
	, height(0.f)
	, radius(0.f)
{}

inline ZACylinder::ZACylinder(const ZACylinder & copy)
	: centerPosition(copy.centerPosition)
	, height(copy.height)
	, radius(copy.radius)
{
}

inline ZACylinder::ZACylinder(float centerX, float centerY, float centerZ, float _height, float _radius)
	: centerPosition(centerX,centerY,centerZ)
	, height(_height)
	, radius(_radius)
{
}

inline ZACylinder::ZACylinder(const Vector3 & centerPos, float _height, float _radius)
	: centerPosition(centerPos)
	, height(_height)
	, radius(_radius)
{
}

inline Vector3 ZACylinder::GetBottomPosition() const
{
	return centerPosition + Vector3(0.f, 0.f, -height*0.5f);
}

inline Vector3 ZACylinder::GetTopPosition() const
{
	return centerPosition + Vector3(0.f, 0.f, height*0.5f);
}
