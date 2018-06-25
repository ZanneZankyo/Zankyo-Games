#pragma once
#include "Engine/Math/Vector3.hpp"

class Ray3
{
public:
	Vector3 start;
	Vector3 end;

public:
	Ray3();
	Ray3(const Ray3& copy);
	explicit Ray3(float startX, float startY, float startZ, float endX, float endY, float endZ);
	explicit Ray3(Vector3 a_start, Vector3 a_end);
	//explicit Ray3(Vector3 a_start, float length, float headingDegrees);
	~Ray3();

	//Mutators:
	Vector3 SetToOrigin();
	void SetStartAndEnd(const Vector3& a_start, const Vector3& a_end);
	float SetLength(float length);
	float AddLength(float lengthToAdd);
/*
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float length, float headingDegrees);
	void SetLengthAndHeadingRadians(float length, float headingRadians);
	void Rotate90Degrees();
	void Ratate180Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float rotateDegrees);
	void RotateRadians(float rotateRadians);*/
	void Translate(const Vector3& translation);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector3& perAxisDivisors);

	//Accessors / queries:
	const float CalcLength() const;
	const Vector3 CalcCenter() const;
	const Vector3 CalcVector() const;

	//Operators
	const Ray3 operator = (const Ray3& copy);
	bool operator == (const Ray3& rayToEqual) const;
	bool operator != (const Ray3& rayToNotEqual) const;
	const Ray3 operator + (const Vector3& translation) const;
	const Ray3 operator + (const Ray3& translation) const;
	const Ray3 operator - (const Vector3& antiTranslation) const;
	const Ray3 operator * (float scale) const;
	const Ray3 operator * (const Vector3& perAxisScaleFactors) const;
	const Ray3 operator / (float inverseScale) const;
	const Ray3 operator / (const Vector3 & perAxisInverseScaleFactors) const;
	void operator *= (float scale);
	void operator *= (const Vector3& perAxisScaleFactors);
	void operator += (const Vector3& translation);
	void operator -= (const Vector3& antiTranslation);

	friend const Ray3 operator * (float scale, const Ray3& rayToScale);
	friend bool IsParallel(const Ray3& a, const Ray3& b);
};