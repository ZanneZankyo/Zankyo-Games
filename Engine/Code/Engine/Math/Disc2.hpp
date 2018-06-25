#pragma once
#include "Engine/Math/Vector2.hpp"

class Disc2
{
public:
	Vector2 position;
	float radius;

	static const Disc2 UNIT_CIRCLE; // Disc2D::UNIT_CIRCLE; center (0,0) and radius 1

public:
	Disc2();
	Disc2(const Disc2& copy);
	explicit Disc2(float initialX, float initialY, float initialRadius);
	explicit Disc2(const Vector2& initialCenter, float initialRadius);
	~Disc2() {}

	//Mutators:
	void StretchToIncludePoint(const Vector2& point);
	void AddPadding(float paddingRadius);
	void Translate(float x, float y);
	void Translate(const Vector2& translation);

	//Accessors / queries:
	bool IsPointInside(const Vector2& point) const;

	//Operators:
	const Disc2 operator + (const Vector2& translation) const;
	const Disc2 operator - (const Vector2& antiTranslation) const;
	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);


	//friend
	friend bool DoDisc2sOverlap(const Disc2& a, const Disc2& b);
	friend bool DoDisc2sOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius);
	friend const Disc2 Interpolate(const Disc2& start, const Disc2& end, float fractionToEnd);

};