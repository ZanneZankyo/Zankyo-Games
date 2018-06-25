#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <complex>
#include <iostream>
#include "Vector3.hpp"

Vector4::Vector4():x(0.f),y(0.f),z(0.f),w(1.f){}

Vector4::Vector4(const Vector4 & copy):x(copy.x),y(copy.y),z(copy.z),w(copy.w){}

Vector4::Vector4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX), y(initialY), z(initialZ), w(initialW) {}

Vector4::Vector4(const Vector3& vector3, float w /*= 1.f*/)
	: x(vector3.x), y(vector3.y), z(vector3.z), w(w) {}
const Vector4 Vector4::ZERO(0.f, 0.f, 0.f, 0.f);
const Vector4 Vector4::ONE(1.f, 1.f, 1.f, 1.f);
const Vector4 Vector4::UNIT(1.f, 0.f, 0.f, 1.f);

void Vector4::GetXYZW(float & out_x, float & out_y, float & out_z, float& out_w) const
{
	out_x = x;
	out_y = y;
	out_z = z;
	out_w = w;
}

const float * Vector4::GetAsFloatArray() const
{
	return &x;
}

void Vector4::SetXYZW(float newX, float newY, float newZ, float newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

/*
void Vector4::Rotate90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = -1 * oldY;
	y = oldX;
}

void Vector4::Ratate180Degrees()
{
	x *= -1;
	y *= -1;
}

void Vector4::RotateNegative90Degrees()
{
	float oldX = x;
	float oldY = y;
	x = oldY;
	y = -1 * oldX;
}

void Vector4::RotateDegrees(float degrees)
{
	float r = CalcLength();
	float oldDegrees = CalcHeadingDegrees();
	float newDegrees = oldDegrees + degrees;
	x = r * CosDegrees(newDegrees);
	y = r * SinDegrees(newDegrees);
}

void Vector4::RotateRadians(float radians)
{
	float r = CalcLength();
	float oldRadians = CalcHeadingRadians();
	float newRadians = oldRadians + radians;
	x = r * cos(newRadians);
	y = r * sin(newRadians);
}*/

float Vector4::Normalize3D()
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

float Vector4::Normalize4D()
{
	float length = CalcLength();
	if (length > 0.f) 
	{
		float scale = 1.f / length;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
	return length;
}

Vector4 Vector4::GetNormalize()
{
	Vector4 normalize = Vector4(*this);
	normalize.Normalize4D();
	return normalize;
}

float Vector4::SetLength(float newLength)
{
	float oldLength = CalcLength();
	Normalize4D();
	x *= newLength;
	y *= newLength;
	z *= newLength;
	w *= newLength;
	return oldLength;
}

void Vector4::ScaleUniform(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void Vector4::ScaleNonUniform(const Vector4 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void Vector4::InverseScaleNonUniform(const Vector4 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

/*
void Vector4::SetHeadingDegrees(float headingDegrees)
{
	float length = CalcLength();
	x = length * CosDegrees(headingDegrees);
	y = length * SinDegrees(headingDegrees);
}

void Vector4::SetHeadingRadians(float headingRadians)
{
	float length = CalcLength();
	x = length * cos(headingRadians);
	y = length * sin(headingRadians);
}

void Vector4::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	x = CosDegrees(headingDegrees);
	y = SinDegrees(headingDegrees);
}

void Vector4::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	x = cos(headingRadians);
	y = sin(headingRadians);
}

void Vector4::SetLengthAndHeadingDegrees(float newLength, float headingDegrees)
{
	x = newLength * CosDegrees(headingDegrees);
	y = newLength * SinDegrees(headingDegrees);
}
			
void Vector4::SetLengthAndHeadingRadians(float newLength, float headingRadians)
{
	x = newLength * cos(headingRadians);
	y = newLength * sin(headingRadians);
}*/

bool Vector4::IsMostlyEqualTo(const Vector4 & comparison, float epsilon) const
{
	Vector4 delta = *this - comparison;
	return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon && fabs(delta.w) < epsilon;
}

const Vector4 Vector4::operator=(const Vector4& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
	return *this;
}

bool Vector4::operator==(const Vector4 & vectorToEqual) const
{
	return x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z && w == vectorToEqual.w;
	//return (fabsf(x - vectorToEqual.x) < 0.0000001f && fabsf(y - vectorToEqual.y) < 0.0000001f);
}

bool Vector4::operator!=(const Vector4 & vectorToNotEqual) const
{
	return !(*this == vectorToNotEqual);
}

const Vector4 Vector4::operator+(const Vector4 & vectorToAdd) const
{
	return Vector4(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z, w + vectorToAdd.w);
}

const Vector4 Vector4::operator-(const Vector4 & vectorToSubtract) const
{
	return Vector4(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z, w - vectorToSubtract.w);
}

const Vector4 Vector4::operator*(float scale) const
{
	return Vector4(x * scale, y * scale, z * scale, w * scale);
}

const Vector4 Vector4::operator*(const Vector4 & perAxisScaleFactors) const
{
	return Vector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const Vector4 Vector4::operator/(float inverseScale) const
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}
	else
		return Vector4::ZERO;
}

void Vector4::operator*=(float scale)
{
	ScaleUniform(scale);
}

void Vector4::operator*=(const Vector4 & perAxisScaleFactors)
{
	ScaleNonUniform(perAxisScaleFactors);
}

void Vector4::operator/=(float inverseScale)
{
	if (inverseScale != 0)
	{
		float scale = 1.f / inverseScale;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
}

void Vector4::operator/=(const Vector4 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

void Vector4::operator+=(const Vector4 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
	w += vectorToAdd.w;
}

void Vector4::operator-=(const Vector4 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	w -= vectorToSubtract.w;
}

/*
Vector4 Vector4::GetNormalizeFormDegrees(float degrees)
{
	Vector4 vector = Vector4::UNIT;
	vector.SetHeadingDegrees(degrees);
	return vector;
}*/

float CalcDistance(const Vector4 & positionA, const Vector4 & positionB)
{
	return sqrtf(CalcDistanceSquared(positionA, positionB));
}

float CalcDistanceSquared(const Vector4 & positionA, const Vector4 & positionB)
{
	float dx = positionA.x - positionB.x;
	float dy = positionA.y - positionB.y;
	float dz = positionA.z - positionB.z;
	float dw = positionA.w - positionB.w;
	return (dx * dx) + (dy * dy) + (dz * dz) + (dw * dw);
}

const Vector4 operator*(float scale, const Vector4 & vectorToScale)
{
	return Vector4(vectorToScale * scale);
}

float DotProduct(const Vector4 & a, const Vector4 & b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	return result;
}

bool AreMostlyEqual(const Vector4 & a, const Vector4 & b, float epsilon)
{
	//Vector4 delta = a - b;
	//return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon;
	return a.IsMostlyEqualTo(b,epsilon);
}

std::ostream & operator<<(std::ostream & out, const Vector4 & pos)
{
	out << "Vector4( " << pos.x << " , " << pos.y << " , " << pos.z << " , " << pos.w << " )";
	return out;
}

float * Vector4::GetAsFloatArray()
{
	return &x;
}

float Vector4::CalcLength() const
{
	return sqrtf(CalcLengthSquared());
}

float Vector4::CalcLengthSquared() const
{
	return (x*x) + (y*y) + (z*z) + (w*w);
}

/*
float Vector4::CalcHeadingDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vector4::CalcHeadingRadians() const
{
	return atan2(y,x);
}*/
