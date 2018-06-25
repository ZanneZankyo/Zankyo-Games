#pragma once
#include "Engine/Math/Vector2.hpp"

class Ray2
{
public:
	Vector2 start;
	Vector2 end;

public:
	Ray2();
	Ray2(const Ray2& copy);
	explicit Ray2(float startX, float startY, float endX, float endY);
	explicit Ray2(Vector2 a_start, Vector2 a_end);
	explicit Ray2(Vector2 a_start, float length, float headingDegrees);
	~Ray2();

	//Mutators:
	Vector2 SetToOrigin();
	void SetStartAndEnd(const Vector2& a_start, const Vector2& a_end);
	float SetLength(float length);
	float AddLength(float lengthToAdd);
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float length, float headingDegrees);
	void SetLengthAndHeadingRadians(float length, float headingRadians);
	void Rotate90Degrees();
	void Ratate180Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float rotateDegrees);
	void RotateRadians(float rotateRadians);
	void Translate(const Vector2& translation);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector2& perAxisDivisors);

	//Accessors / queries:
	const float CalcLength() const;
	const Vector2 CalcCenter() const;
	const Vector2 CalcVector() const;

	//Operators
	const Ray2 operator = (const Ray2& copy);
	bool operator == (const Ray2& rayToEqual) const;
	bool operator != (const Ray2& rayToNotEqual) const;
	const Ray2 operator + (const Vector2& translation) const;
	const Ray2 operator + (const Ray2& translation) const;
	const Ray2 operator - (const Vector2& antiTranslation) const;
	const Ray2 operator * (float scale) const;
	const Ray2 operator * (const Vector2& perAxisScaleFactors) const;
	const Ray2 operator / (float inverseScale) const;
	const Ray2 operator / (const Vector2 & perAxisInverseScaleFactors) const;
	void operator *= (float scale);
	void operator *= (const Vector2& perAxisScaleFactors);
	void operator += (const Vector2& translation);
	void operator -= (const Vector2& antiTranslation);

	friend const Ray2 operator * (float scale, const Ray2& rayToScale);
	friend bool IsParallel(const Ray2& a, const Ray2& b);
};