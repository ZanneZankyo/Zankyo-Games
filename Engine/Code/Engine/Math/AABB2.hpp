#pragma once

#include "Engine/Math/Vector2.hpp"

class AABB2
{
public:
	Vector2 mins;
	Vector2 maxs;

	static const AABB2 ZERO_TO_ONE; 
	static const AABB2 NEG_ONE_TO_ONE;

public:

	AABB2();
	AABB2(const AABB2& copy);
	explicit AABB2(float initialX, float initialY);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(const Vector2& mins, const Vector2& maxs);
	explicit AABB2(const Vector2& center, float radiusX, float radiusY);
	~AABB2();

	//Mutators:
	void StretchToIncludePoint(const Vector2& point);
	void AddPaddingToSides(float xPaddingRadius, float yPaddingRadius);
	void Translate(const Vector2& translation);

	//Accessors / queries:
	bool IsPointInside(const Vector2& point) const;
	const Vector2 CalcSize() const; // or perhaps CalcDimensions
	const Vector2 CalcCenter() const;

	Vector2 GetUV(const Vector2& position) const;
	Vector2 EvaluateFromUV(const Vector2& uv) const;

	//Operators:
	const AABB2 operator + (const Vector2& translation) const;
	const AABB2 operator - (const Vector2& antiTranslation) const;

	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);

	bool operator == (const AABB2& boundsToCompare) const;
	bool operator != (const AABB2& boundsToCompare) const;

	//Construct form center-extends
	static AABB2 CreateFromCenterAndSize(const Vector2& center, const Vector2& size);

	//friend
	friend bool DoAABB2sOverlap(const AABB2& a, const AABB2& b);
	friend const AABB2 Interpolate(const AABB2& start, const AABB2& end, float fractionToEnd);
};