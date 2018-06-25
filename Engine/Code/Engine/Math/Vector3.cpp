#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <complex>
#include <iostream>
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Matrix4.hpp"

const Vector3 Vector3::ZERO(0.f, 0.f, 0.f);
const Vector3 Vector3::ONE(1.f, 1.f, 1.f);
const Vector3 Vector3::UNIT(1.f, 0.f, 0.f);
const Vector3 Vector3::X_AXIS(1.f, 0.f, 0.f);
const Vector3 Vector3::Y_AXIS(0.f, 1.f, 0.f);
const Vector3 Vector3::Z_AXIS(0.f, 0.f, 1.f);


Vector3::Vector3(const Vector2 & copy)
	:x(copy.x)
	,y(copy.y)
	,z(0.f)
{
}


Vector3::Vector3(const Vector4& vector4)
	:x(vector4.x)
	,y(vector4.y)
	,z(vector4.z)
{}

void Vector3::GetXYZ(float & out_x, float & out_y, float & out_z) const
{
	out_x = x;
	out_y = y;
	out_z = z;
}

const float * Vector3::GetAsFloatArray() const
{
	return &x;
}

void Vector3::SetXYZ(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

/*
void Vector3::Rotate90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = -1 * oldY;
	y = oldX;
}

void Vector3::Ratate180Degrees()
{
	x *= -1;
	y *= -1;
}

void Vector3::RotateNegative90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = oldY;
	y = -1 * oldX;
}

void Vector3::RotateDegrees(float degrees)
{
	float r = CalcLength();
	float oldDegrees = CalcHeadingDegrees();
	float newDegrees = oldDegrees + degrees;
	x = r * CosDegrees(newDegrees);
	y = r * SinDegrees(newDegrees);
}

void Vector3::RotateRadians(float radians)
{
	float r = CalcLength();
	float oldRadians = CalcHeadingRadians();
	float newRadians = oldRadians + radians;
	x = r * cos(newRadians);
	y = r * sin(newRadians);
}*/

float Vector3::Normalize()
{
	float length = CalcLength();
	if (length > 0.f) 
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
		z *= scale;
	}
	return length;
}

Vector3 Vector3::GetNormalize() const
{
	Vector3 normalize = Vector3(*this);
	normalize.Normalize();
	return normalize;
}

float Vector3::SetLength(float newLength)
{
	float oldLength = CalcLength();
	Normalize();
	x *= newLength;
	y *= newLength;
	z *= newLength;
	return oldLength;
}

void Vector3::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Vector3::ScaleNonUniform(const Vector3 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

void Vector3::InverseScaleNonUniform(const Vector3 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

/*
void Vector3::SetHeadingDegrees(float headingDegrees)
{
	float length = CalcLength();
	x = length * CosDegrees(headingDegrees);
	y = length * SinDegrees(headingDegrees);
}

void Vector3::SetHeadingRadians(float headingRadians)
{
	float length = CalcLength();
	x = length * cos(headingRadians);
	y = length * sin(headingRadians);
}

void Vector3::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	x = CosDegrees(headingDegrees);
	y = SinDegrees(headingDegrees);
}

void Vector3::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	x = cos(headingRadians);
	y = sin(headingRadians);
}

void Vector3::SetLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	x = newLength * CosDegrees(headingDegrees);
	y = newLength * SinDegrees(headingDegrees);
}
			
void Vector3::SetLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * cos(headingRadians);
	y = newLength * sin(headingRadians);
}*/

bool Vector3::IsMostlyEqualTo(const Vector3 & comparison, float epsilon) const
{
	Vector3 delta = *this - comparison;
	return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon;
}



/*
Vector3 Vector3::GetNormalizeFormDegrees(float degrees)
{
	Vector3 vector = Vector3::UNIT;
	vector.SetHeadingDegrees(degrees);
	return vector;
}*/

float CalcDistance(const Vector3 & positionA, const Vector3 & positionB)
{
	return sqrtf(CalcDistanceSquared(positionA, positionB));
}

float CalcDistanceSquared(const Vector3 & positionA, const Vector3 & positionB)
{
	float dx = positionA.x - positionB.x;
	float dy = positionA.y - positionB.y;
	float dz = positionA.z - positionB.z;
	return (dx * dx) + (dy * dy) + (dz * dz);
}



float * Vector3::GetAsFloatArray()
{
	return &x;
}

float Vector3::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

float Vector3::CalcLengthSquared() const
{
	return (x*x) + (y*y) + (z*z);
}

/*
float Vector3::CalcHeadingDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vector3::CalcHeadingRadians() const
{
	return atan2(y,x);
}*/

const Vector3 Vector3::operator*(const Matrix4 & mat) const
{
	return mat.TransformVector(Vector4(*this));
}