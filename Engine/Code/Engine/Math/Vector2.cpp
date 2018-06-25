#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <complex>
#include <iostream>

const Vector2 Vector2::ZERO(0.f, 0.f);
const Vector2 Vector2::ONE(1.f, 1.f);
const Vector2 Vector2::UNIT(1.f, 0.f);



void Vector2::RotateDegrees(float degrees)
{
	float r = CalcLength();
	float oldDegrees = CalcHeadingDegrees();
	float newDegrees = oldDegrees + degrees;
	x = r * CosDegrees(newDegrees);
	y = r * SinDegrees(newDegrees);
}

void Vector2::RotateRadians(float radians)
{
	float r = CalcLength();
	float oldRadians = CalcHeadingRadians();
	float newRadians = oldRadians + radians;
	x = r * cos(newRadians);
	y = r * sin(newRadians);
}

float Vector2::Normalize()
{
	float length = CalcLength();
	if (length > 0.f) 
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
	}
	return length;
}

Vector2 Vector2::GetNormalize()
{
	Vector2 normalize = Vector2(*this);
	normalize.Normalize();
	return normalize;
}

float Vector2::SetLength(float newLength)
{
	float oldLength = CalcLength();
	Normalize();
	x *= newLength;
	y *= newLength;
	return oldLength;
}

void Vector2::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
}

void Vector2::ScaleNonUniform(const Vector2 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

void Vector2::InverseScaleNonUniform(const Vector2 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

void Vector2::SetHeadingDegrees(float headingDegrees)
{
	float length = CalcLength();
	x = length * CosDegrees(headingDegrees);
	y = length * SinDegrees(headingDegrees);
}

void Vector2::SetHeadingRadians(float headingRadians)
{
	float length = CalcLength();
	x = length * cos(headingRadians);
	y = length * sin(headingRadians);
}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	x = CosDegrees(headingDegrees);
	y = SinDegrees(headingDegrees);
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	x = cos(headingRadians);
	y = sin(headingRadians);
}

void Vector2::SetLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	x = newLength * CosDegrees(headingDegrees);
	y = newLength * SinDegrees(headingDegrees);
}
			
void Vector2::SetLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * cos(headingRadians);
	y = newLength * sin(headingRadians);
}

bool Vector2::IsMostlyEqualTo(const Vector2 & comparison, float epsilon)
{
	Vector2 delta = *this - comparison;
	return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon;
}



float * Vector2::GetAsFloatArray()
{
	/*float *floatArray = new float[2]{ x,y };
	return floatArray;*/
	return &x;
}

float Vector2::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

float Vector2::CalcLengthSquared() const
{
	return (x*x) + (y*y);
}

float Vector2::CalcHeadingDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vector2::CalcHeadingRadians() const
{
	return atan2(y,x);
}
