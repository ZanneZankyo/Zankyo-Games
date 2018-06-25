#pragma once
#include <iostream>

class Vector3;

class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	static const Vector4 ZERO;
	static const Vector4 ONE;
	static const Vector4 UNIT;

public:
	//Constructors
	Vector4();
	Vector4(const Vector4& copy);
	explicit Vector4(float initialX, float initialY, float initialZ, float initialW);
	explicit Vector4(const Vector3& vector3, float w = 1.f);

	//Accessors
	void GetXYZW(float& out_x, float& out_y, float& out_z, float& out_w) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
// 	float CalcHeadingDegrees() const;
// 	float CalcHeadingRadians() const;

	//Mutators
	void SetXYZW(float newX, float newY, float newZ, float newW);
// 	void Rotate90Degrees();
// 	void Ratate180Degrees();
// 	void RotateNegative90Degrees();
// 	void RotateDegrees(float degrees);
// 	void RotateRadians(float radians);
	float Normalize3D();
	float Normalize4D();
	Vector4 GetNormalize();
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector4& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector4& perAxisDivisors);
// 	void SetHeadingDegrees(float headingDegrees);
// 	void SetHeadingRadians(float headingRadians);
// 	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
// 	void SetUnitLengthAndHeadingRadians(float headingRadians);
// 	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
// 	void SetLengthAndHeadingRadians(float newLength, float headingRadians);
	bool IsMostlyEqualTo(const Vector4& comparison, float epsilon = 0.001f) const;

	//Operators
	const Vector4 operator = (const Vector4& copy);
	bool operator == (const Vector4& vectorToEqual) const;
	bool operator != (const Vector4& vectorToNotEqual) const;
	const Vector4 operator + (const Vector4& vectorToAdd) const;
	const Vector4 operator - (const Vector4& vectorToSubtract) const;
	const Vector4 operator * (float scale) const;
	const Vector4 operator * (const Vector4& perAxisScaleFactors) const;
	const Vector4 operator / (float inverseScale) const;
	void operator *= (float scale);
	void operator *= (const Vector4& perAxisScaleFactors);
	void operator /= (float inverseScale);
	void operator /= (const Vector4& perAxisDivisors);
	void operator += (const Vector4& vectorToAdd);
	void operator -= (const Vector4& vectorToSubtract);

	//static
	//static Vector4 GetNormalizeFormDegrees(float degrees);

	//Standalone "friend" functions
	friend float CalcDistance(const Vector4& positionA, const Vector4& positionB);
	friend float CalcDistanceSquared(const Vector4& positionA, const Vector4& positionB);
	friend const Vector4 operator * (float scale, const Vector4& vectorToScale);
	friend float DotProduct(const Vector4& a, const Vector4& b);
	friend bool AreMostlyEqual(const Vector4& a, const Vector4& b, float epsilon = 0.001f);
	friend std::ostream& operator << (std::ostream& out ,const Vector4& pos);

};