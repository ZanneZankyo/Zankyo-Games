#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere3
{
public:
	Vector3 position;
	float radius;

	static const Sphere3 UNIT_SPHERE; // center (0,0,0) and radius 1

public:
	Sphere3();
	Sphere3(const Sphere3& copy);
	explicit Sphere3(float initialX, float initialY, float initialZ, float initialRadius);
	explicit Sphere3(const Vector3& initialCenter, float initialRadius);
	~Sphere3() {}

	//Mutators:
	void StretchToIncludePoint(const Vector3& point);
	void AddPadding(float paddingRadius);
	void Translate(float x, float y, float z);
	void Translate(const Vector3& translation);

	//Accessors / queries:
	bool IsPointInside(const Vector3& point) const;

	//Operators:
	const Sphere3 operator + (const Vector3& translation) const;
	const Sphere3 operator - (const Vector3& antiTranslation) const;
	void operator += (const Vector3& translation);
	void operator -= (const Vector3& antiTranslation);

	//friend
	friend bool DoSphere3sOverlap(const Sphere3& a, const Sphere3& b);
	friend const Sphere3 Interpolate(const Sphere3& start, const Sphere3& end, float fractionToEnd);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////


inline Sphere3::Sphere3()
	: position(Vector3(0.f, 0.f, 0.f))
	, radius(0.f)
{}

inline Sphere3::Sphere3(const Sphere3 & copy)
	: position(copy.position)
	, radius(copy.radius)
{}

inline Sphere3::Sphere3(float initialX, float initialY, float initialZ, float initialRadius)
	: position(initialX, initialY, initialZ)
	, radius(initialRadius)
{}

inline Sphere3::Sphere3(const Vector3& initialCenter, float initialRadius)
	: position(initialCenter)
	, radius(initialRadius)
{}

inline void Sphere3::StretchToIncludePoint(const Vector3 & point)
{
	if (IsPointInside(point))
		return;
	float distance = CalcDistance(position, point);
	radius = distance;
}

inline void Sphere3::AddPadding(float paddingRadius)
{
	radius += paddingRadius;
}

inline void Sphere3::Translate(float x, float y, float z)
{
	position += Vector3(x, y, z);
}

inline void Sphere3::Translate(const Vector3 & translation)
{
	position += translation;
}

inline bool Sphere3::IsPointInside(const Vector3 & point) const
{
	float distanceSquared = CalcDistanceSquared(position, point);
	return distanceSquared < radius * radius;
}

inline const Sphere3 Sphere3::operator+(const Vector3 & translation) const
{
	return Sphere3(position + translation, radius);
}

inline const Sphere3 Sphere3::operator-(const Vector3 & antiTranslation) const
{
	return Sphere3(position - antiTranslation, radius);
}

inline void Sphere3::operator+=(const Vector3 & translation)
{
	position += translation;
}

inline void Sphere3::operator-=(const Vector3 & antiTranslation)
{
	position -= antiTranslation;
}