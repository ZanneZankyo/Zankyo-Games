#pragma once
#include <iostream>
#include "IntVector2.hpp"
#include "Vector3.hpp"
#include <iomanip>

class Vector2
{
public:
	float x;
	float y;

	static const Vector2 ZERO;
	static const Vector2 ONE;
	static const Vector2 UNIT;

public:
	//Constructors
	Vector2();
	Vector2(const Vector2& copy);
	explicit Vector2(const IntVector2& copy);
	explicit Vector2(const Vector3& copy);
	explicit Vector2(float initialX, float initialY);
	explicit Vector2(int initialX, int initialY);

	//Accessors
	void GetXY(float& out_x, float& out_y) const;
	const float* GetAsFloatArray() const;
	float* GetAsFloatArray();
	float CalcLength() const;
	float CalcLengthSquared() const;
	float CalcHeadingDegrees() const;
	float CalcHeadingRadians() const;

	//Mutators
	void SetXY(float newX, float newY);
	void Rotate90Degrees();
	void Ratate180Degrees();
	void RotateNegative90Degrees();
	void RotateDegrees(float degrees);
	void RotateRadians(float radians);
	float Normalize();
	Vector2 GetNormalize();
	float SetLength(float newLength);
	void ScaleUniform(float scale);
	void ScaleNonUniform(const Vector2& perAxisScaleFactors);
	void InverseScaleNonUniform(const Vector2& perAxisDivisors);
	void SetHeadingDegrees(float headingDegrees);
	void SetHeadingRadians(float headingRadians);
	void SetUnitLengthAndHeadingDegrees(float headingDegrees);
	void SetUnitLengthAndHeadingRadians(float headingRadians);
	void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
	void SetLengthAndHeadingRadians(float newLength, float headingRadians);
	bool IsMostlyEqualTo(const Vector2& comparison, float epsilon = 0.001f);

	//Operators
	const Vector2 operator = (const Vector2& copy);
	bool operator == (const Vector2& vectorToEqual) const;
	bool operator != (const Vector2& vectorToNotEqual) const;
	const Vector2 operator + (const Vector2& vectorToAdd) const;
	const Vector2 operator - (const Vector2& vectorToSubtract) const;
	const Vector2 operator * (float scale) const;
	const Vector2 operator * (const Vector2& perAxisScaleFactors) const;
	const Vector2 operator / (float inverseScale) const;
	const Vector2 operator / (const Vector2& perAxisInverseScaleFactors) const;
	const Vector2 operator - () const;
	void operator *= (float scale);
	void operator *= (const Vector2& perAxisScaleFactors);
	void operator /= (float inverseScale);
	void operator /= (const Vector2& perAxisDivisors);
	void operator += (const Vector2& vectorToAdd);
	void operator -= (const Vector2& vectorToSubtract);

	//static
	static Vector2 GetNormalizeFormDegrees(float degrees);

	//Standalone "friend" functions
	friend float CalcDistance(const Vector2& positionA, const Vector2& positionB);
	friend float CalcDistanceSquared(const Vector2& positionA, const Vector2& positionB);
	friend const Vector2 operator * (float scale, const Vector2& vectorToScale);
	friend float DotProduct(const Vector2& a, const Vector2& b);
	friend bool AreMostlyEqual(const Vector2& a, const Vector2& b, float epsilon = 0.001f);
	friend std::ostream& operator << (std::ostream& out ,const Vector2& pos);
	friend float CompareSlope(const Vector2& a, const Vector2& b);
};


//------------------------------

inline Vector2::Vector2() :x(0.f), y(0.f) {}

inline Vector2::Vector2(const Vector2 & copy) : x(copy.x), y(copy.y) {}

inline Vector2::Vector2(const IntVector2 & copy)
	: x(static_cast<float>(copy.x))
	, y(static_cast<float>(copy.y))
{}

inline Vector2::Vector2(const Vector3 & copy)
	: x(copy.x)
	, y(copy.y)
{}

inline Vector2::Vector2(float initialX, float initialY) : x(initialX), y(initialY) {}

inline Vector2::Vector2(int initialX, int initialY)
	: x(static_cast<float>(initialX))
	, y(static_cast<float>(initialY))
{}

inline void Vector2::GetXY(float & out_x, float & out_y) const
{
	out_x = x;
	out_y = y;
}

inline const float * Vector2::GetAsFloatArray() const
{
	return &x;
}

inline void Vector2::SetXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

inline void Vector2::Rotate90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = -1 * oldY;
	y = oldX;
}

inline void Vector2::Ratate180Degrees()
{
	x *= -1;
	y *= -1;
}

inline void Vector2::RotateNegative90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = oldY;
	y = -1 * oldX;
}

inline const Vector2 Vector2::operator=(const Vector2& copy)
{
	x = copy.x;
	y = copy.y;
	return *this;
}

inline bool Vector2::operator==(const Vector2 & vectorToEqual) const
{
	return x == vectorToEqual.x && y == vectorToEqual.y;
	//return (fabsf(x - vectorToEqual.x) < 0.0000001f && fabsf(y - vectorToEqual.y) < 0.0000001f);
}

inline bool Vector2::operator!=(const Vector2 & vectorToNotEqual) const
{
	return !(*this == vectorToNotEqual);
}

inline const Vector2 Vector2::operator+(const Vector2 & vectorToAdd) const
{
	return Vector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

inline const Vector2 Vector2::operator-(const Vector2 & vectorToSubtract) const
{
	return Vector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

inline const Vector2 Vector2::operator*(float scale) const
{
	return Vector2(x * scale, y * scale);
}

inline const Vector2 Vector2::operator*(const Vector2 & perAxisScaleFactors) const
{
	return Vector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline const Vector2 Vector2::operator/(float inverseScale) const
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		return Vector2(x * scale, y * scale);
	}
	else
		return Vector2::ZERO;
}

inline const Vector2 Vector2::operator/(const Vector2& perAxisInverseScaleFactors) const
{
	if (perAxisInverseScaleFactors.x != 0 && perAxisInverseScaleFactors.y != 0)
	{
		return Vector2(x / perAxisInverseScaleFactors.x, y / perAxisInverseScaleFactors.y);
	}
	else
		return Vector2::ZERO;
}

inline const Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

inline void Vector2::operator*=(float scale)
{
	ScaleUniform(scale);
}

inline void Vector2::operator*=(const Vector2 & perAxisScaleFactors)
{
	ScaleNonUniform(perAxisScaleFactors);
}

inline void Vector2::operator/=(float inverseScale)
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		x *= scale;
		y *= scale;
	}
}

inline void Vector2::operator/=(const Vector2 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

inline void Vector2::operator+=(const Vector2 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

inline void Vector2::operator-=(const Vector2 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

inline Vector2 Vector2::GetNormalizeFormDegrees(float degrees)
{
	Vector2 vector = Vector2::UNIT;
	vector.SetHeadingDegrees(degrees);
	return vector;
}

inline float CalcDistance(const Vector2 & positionA, const Vector2 & positionB)
{
	return sqrtf(CalcDistanceSquared(positionA, positionB));
}

inline float CalcDistanceSquared(const Vector2 & positionA, const Vector2 & positionB)
{
	float dx = positionA.x - positionB.x;
	float dy = positionA.y - positionB.y;
	return (dx * dx) + (dy * dy);
}

inline const Vector2 operator*(float scale, const Vector2 & vectorToScale)
{
	return Vector2(vectorToScale * scale);
}

inline float DotProduct(const Vector2 & a, const Vector2 & b)
{
	float result = a.x * b.x + a.y * b.y;
	return result;
}

inline bool AreMostlyEqual(const Vector2 & a, const Vector2 & b, float epsilon)
{
	Vector2 delta = a - b;
	return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon;
}

inline std::ostream & operator<<(std::ostream & out, const Vector2 & pos)
{
	out << "Vector2( "<< std::setprecision(2) << pos.x << " , "<< std::setprecision(2) << pos.y << " )";
	return out;
}

inline float CompareSlope(const Vector2 & a, const Vector2 & b)
{
	return a.y * b.x - b.y * a.x;
}