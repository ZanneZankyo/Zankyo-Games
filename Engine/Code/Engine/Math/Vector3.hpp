#pragma once
#include <iostream>
#include "IntVector3.hpp"
#include <iomanip>
#include "Vector4.hpp"

class Matrix4;

class Vector3
{
public:

	friend class Vector2;
	friend class Vector4;

	float x;
	float y;
	float z;

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 UNIT;
	static const Vector3 X_AXIS;
	static const Vector3 Y_AXIS;
	static const Vector3 Z_AXIS;

public:
	//Constructors
	Vector3();
	Vector3(const Vector3& copy);
	Vector3(const IntVector3& copy);
	Vector3(const Vector2& copy);
	Vector3(const Vector4& vector4);
	explicit Vector3(float initialX, float initialY, float initialZ);
	explicit Vector3(int initialX, int initialY, int initialZ);

	//Accessors
	void GetXYZ(float& out_x, float& out_y, float& out_z) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
// 	float CalcHeadingDegrees() const;
// 	float CalcHeadingRadians() const;

	//Mutators
	void SetXYZ(float newX, float newY, float newZ);
// 	void Rotate90Degrees();
// 	void Ratate180Degrees();
// 	void RotateNegative90Degrees();
// 	void RotateDegrees(float degrees);
// 	void RotateRadians(float radians);
	float Normalize();
	Vector3 GetNormalize() const;
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector3& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector3& perAxisDivisors);
// 	void SetHeadingDegrees(float headingDegrees);
// 	void SetHeadingRadians(float headingRadians);
// 	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
// 	void SetUnitLengthAndHeadingRadians(float headingRadians);
// 	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
// 	void SetLengthAndHeadingRadians(float newLength, float headingRadians);
	bool IsMostlyEqualTo(const Vector3& comparison, float epsilon = 0.001f) const;

	//Operators
	const Vector3 operator = (const Vector3& copy);
	bool operator == (const Vector3& vectorToEqual) const;
	bool operator != (const Vector3& vectorToNotEqual) const;
	const Vector3 operator + (const Vector3& vectorToAdd) const;
	const Vector3 operator - (const Vector3& vectorToSubtract) const;
	const Vector3 operator * (float scale) const;
	const Vector3 operator * (const Vector3& perAxisScaleFactors) const;
	const Vector3 operator * (const Matrix4& mat) const;
	const Vector3 operator / (float inverseScale) const;
	const Vector3 operator - () const;
	void operator *= (float scale);
	void operator *= (const Vector3& perAxisScaleFactors);
	void operator /= (float inverseScale);
	void operator /= (const Vector3& perAxisDivisors);
	void operator += (const Vector3& vectorToAdd);
	void operator -= (const Vector3& vectorToSubtract);

	//static
	//static Vector3 GetNormalizeFormDegrees(float degrees);

	//Standalone "friend" functions
	friend float CalcDistance(const Vector3& positionA, const Vector3& positionB);
	friend float CalcDistanceSquared(const Vector3& positionA, const Vector3& positionB);
	friend const Vector3 operator * (float scale, const Vector3& vectorToScale);
	friend float DotProduct(const Vector3& a, const Vector3& b);
	friend Vector3 CrossProduct(const Vector3& a, const Vector3& b);
	friend bool AreMostlyEqual(const Vector3& a, const Vector3& b, float epsilon = 0.001f);
	friend std::ostream& operator << (std::ostream& out ,const Vector3& pos);

};

inline Vector3::Vector3() :x(0.f), y(0.f), z(0.f) {}

inline Vector3::Vector3(const Vector3 & copy) : x(copy.x), y(copy.y), z(copy.z) {}

inline Vector3::Vector3(const IntVector3 & copy)
	: x(static_cast<float>(copy.x))
	, y(static_cast<float>(copy.y))
	, z(static_cast<float>(copy.z))
{}

inline Vector3::Vector3(float initialX, float initialY, float initialZ) :x(initialX), y(initialY), z(initialZ) {}

inline Vector3::Vector3(int initialX, int initialY, int initialZ)
	: x(static_cast<float>(initialX))
	, y(static_cast<float>(initialY))
	, z(static_cast<float>(initialZ))
{}

inline const Vector3 Vector3::operator=(const Vector3& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	return *this;
}

inline bool Vector3::operator==(const Vector3 & vectorToEqual) const
{
	return x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z;
	//return (fabsf(x - vectorToEqual.x) < 0.0000001f && fabsf(y - vectorToEqual.y) < 0.0000001f);
}

inline bool Vector3::operator!=(const Vector3 & vectorToNotEqual) const
{
	return !(*this == vectorToNotEqual);
}

inline const Vector3 Vector3::operator+(const Vector3 & vectorToAdd) const
{
	return Vector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

inline const Vector3 Vector3::operator-(const Vector3 & vectorToSubtract) const
{
	return Vector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

inline const Vector3 Vector3::operator*(float scale) const
{
	return Vector3(x * scale, y * scale, z * scale);
}

inline const Vector3 Vector3::operator*(const Vector3 & perAxisScaleFactors) const
{
	return Vector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

inline const Vector3 Vector3::operator/(float inverseScale) const
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		return Vector3(x * scale, y * scale, z * scale);
	}
	else
		return Vector3::ZERO;
}

inline const Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline void Vector3::operator*=(float scale)
{
	ScaleUniform(scale);
}

inline void Vector3::operator*=(const Vector3 & perAxisScaleFactors)
{
	ScaleNonUniform(perAxisScaleFactors);
}

inline void Vector3::operator/=(float inverseScale)
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		x *= scale;
		y *= scale;
		z *= scale;
	}
}

inline void Vector3::operator/=(const Vector3 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

inline void Vector3::operator+=(const Vector3 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

inline void Vector3::operator-=(const Vector3 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}

inline const Vector3 operator*(float scale, const Vector3 & vectorToScale)
{
	return Vector3(vectorToScale * scale);
}

inline float DotProduct(const Vector3 & a, const Vector3 & b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}

inline Vector3 CrossProduct(const Vector3 & a, const Vector3 & b)
{
	float x = a.y * b.z - a.z * b.y;
	float y = a.z * b.x - a.x * b.z;
	float z = a.x * b.y - a.y * b.x;
	return Vector3(x,y,z);
}

inline bool AreMostlyEqual(const Vector3 & a, const Vector3 & b, float epsilon)
{
	//Vector3 delta = a - b;
	//return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon;
	return a.IsMostlyEqualTo(b, epsilon);
}

inline std::ostream & operator<<(std::ostream & out, const Vector3 & pos)
{
	out << "Vector3( " << std::setprecision(2) << pos.x << " , " << std::setprecision(2) << pos.y << " , " << std::setprecision(2) << pos.z << " )";
	return out;
}