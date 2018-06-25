#pragma once
#include "Engine/Math/Vector3.hpp"

class ZOCylinder
{
public:
	Vector3 centerPos;
	float height;
	float radius;

	ZOCylinder();
	ZOCylinder(const ZOCylinder& copy);
	explicit ZOCylinder(const Vector3& p_centerPos, float p_height, float p_radius);
	explicit ZOCylinder(float centerPosX, float centerPosY, float centerPosZ, float p_height, float p_radius);

	Vector3 GetBottomPos() const;
	Vector3 GetTopPos() const;
};

//-----------------------------------------

inline ZOCylinder::ZOCylinder()
	: centerPos()
	, height(0.f)
	, radius(0.f)
{}

inline ZOCylinder::ZOCylinder(const ZOCylinder& copy)
	: centerPos(copy.centerPos)
	, height(copy.height)
	, radius(copy.radius)
{}

inline ZOCylinder::ZOCylinder(const Vector3 & p_centerPos, float p_height, float p_radius)
	: centerPos(p_centerPos)
	, height(p_height)
	, radius(p_radius)
{
}

inline ZOCylinder::ZOCylinder(float centerPosX, float centerPosY, float centerPosZ, float p_height, float p_radius)
	: centerPos(centerPosX,centerPosY,centerPosZ)
	, height(p_height)
	, radius(p_radius)
{
}

inline Vector3 ZOCylinder::GetBottomPos() const
{
	return centerPos - Vector3(0.f, 0.f, height*0.5f);
}

inline Vector3 ZOCylinder::GetTopPos() const
{
	return centerPos + Vector3(0.f, 0.f, height*0.5f);
}
