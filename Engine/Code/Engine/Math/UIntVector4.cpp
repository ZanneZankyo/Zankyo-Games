#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <complex>
#include <iostream>
#include "Vector3.hpp"

UIntVector4::UIntVector4():x(0),y(0),z(0),w(1) {}

UIntVector4::UIntVector4(const UIntVector4 & copy):x(copy.x),y(copy.y),z(copy.z),w(copy.w){}

UIntVector4::UIntVector4(unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW)
	: x(initialX), y(initialY), z(initialZ), w(initialW) {}

const UIntVector4 UIntVector4::ZERO(0, 0, 0, 0);
const UIntVector4 UIntVector4::ONE(1, 1, 1, 1);
const UIntVector4 UIntVector4::UNIT(1, 0, 0, 1);

void UIntVector4::GetXYZW(unsigned int & out_x, unsigned int & out_y, unsigned int & out_z, unsigned int& out_w) const
{
	out_x = x;
	out_y = y;
	out_z = z;
	out_w = w;
}

const unsigned int * UIntVector4::GetAsArray() const
{
	return &x;
}

void UIntVector4::SetXYZW(unsigned int newX, unsigned int newY, unsigned int newZ, unsigned int newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

void UIntVector4::ScaleUniform(unsigned int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

void UIntVector4::ScaleNonUniform(const UIntVector4 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	w *= perAxisScaleFactors.w;
}

void UIntVector4::InverseScaleNonUniform(const UIntVector4 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

/*
void UIntVector4::SetHeadingDegrees(unsigned int headingDegrees)
{
	unsigned int length = CalcLength();
	x = length * CosDegrees(headingDegrees);
	y = length * SinDegrees(headingDegrees);
}

void UIntVector4::SetHeadingRadians(unsigned int headingRadians)
{
	unsigned int length = CalcLength();
	x = length * cos(headingRadians);
	y = length * sin(headingRadians);
}

void UIntVector4::SetUnitLengthAndHeadingDegrees(unsigned int headingDegrees)
{
	x = CosDegrees(headingDegrees);
	y = SinDegrees(headingDegrees);
}

void UIntVector4::SetUnitLengthAndHeadingRadians(unsigned int headingRadians)
{
	x = cos(headingRadians);
	y = sin(headingRadians);
}

void UIntVector4::SetLengthAndHeadingDegrees(unsigned int newLength, unsigned int headingDegrees)
{
	x = newLength * CosDegrees(headingDegrees);
	y = newLength * SinDegrees(headingDegrees);
}
			
void UIntVector4::SetLengthAndHeadingRadians(unsigned int newLength, unsigned int headingRadians)
{
	x = newLength * cos(headingRadians);
	y = newLength * sin(headingRadians);
}*/

bool UIntVector4::IsMostlyEqualTo(const UIntVector4 & comparison, unsigned int epsilon) const
{
	UIntVector4 delta = *this - comparison;
	return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon && fabs(delta.w) < epsilon;
}

const UIntVector4 UIntVector4::operator=(const UIntVector4& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
	w = copy.w;
	return *this;
}

bool UIntVector4::operator==(const UIntVector4 & vectorToEqual) const
{
	return x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z && w == vectorToEqual.w;
	//return (fabsf(x - vectorToEqual.x) < 0.0000001f && fabsf(y - vectorToEqual.y) < 0.0000001f);
}

bool UIntVector4::operator!=(const UIntVector4 & vectorToNotEqual) const
{
	return !(*this == vectorToNotEqual);
}

const UIntVector4 UIntVector4::operator+(const UIntVector4 & vectorToAdd) const
{
	return UIntVector4(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z, w + vectorToAdd.w);
}

const UIntVector4 UIntVector4::operator-(const UIntVector4 & vectorToSubtract) const
{
	return UIntVector4(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z, w - vectorToSubtract.w);
}

const UIntVector4 UIntVector4::operator*(unsigned int scale) const
{
	return UIntVector4(x * scale, y * scale, z * scale, w * scale);
}

const UIntVector4 UIntVector4::operator*(const UIntVector4 & perAxisScaleFactors) const
{
	return UIntVector4(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z, w * perAxisScaleFactors.w);
}

const UIntVector4 UIntVector4::operator/(unsigned int inverseScale) const
{
	if (inverseScale != 0)
	{
		unsigned int scale = 1 / inverseScale;
		return UIntVector4(x * scale, y * scale, z * scale, w * scale);
	}
	else
		return UIntVector4::ZERO;
}

void UIntVector4::operator*=(unsigned int scale)
{
	ScaleUniform(scale);
}

void UIntVector4::operator*=(const UIntVector4 & perAxisScaleFactors)
{
	ScaleNonUniform(perAxisScaleFactors);
}

void UIntVector4::operator/=(unsigned int inverseScale)
{
	if (inverseScale != 0)
	{
		unsigned int scale = 1 / inverseScale;
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
	}
}

void UIntVector4::operator/=(const UIntVector4 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
	w /= perAxisDivisors.w;
}

void UIntVector4::operator+=(const UIntVector4 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
	w += vectorToAdd.w;
}

void UIntVector4::operator-=(const UIntVector4 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	w -= vectorToSubtract.w;
}

/*
UIntVector4 UIntVector4::GetNormalizeFormDegrees(unsigned int degrees)
{
	UIntVector4 vector = UIntVector4::UNIT;
	vector.SetHeadingDegrees(degrees);
	return vector;
}*/

unsigned int CalcDistance(const UIntVector4 & positionA, const UIntVector4 & positionB)
{
	return (unsigned int)sqrt(CalcDistanceSquared(positionA, positionB));
}

unsigned int CalcDistanceSquared(const UIntVector4 & positionA, const UIntVector4 & positionB)
{
	unsigned int dx = positionA.x - positionB.x;
	unsigned int dy = positionA.y - positionB.y;
	unsigned int dz = positionA.z - positionB.z;
	unsigned int dw = positionA.w - positionB.w;
	return (dx * dx) + (dy * dy) + (dz * dz) + (dw * dw);
}

const UIntVector4 operator*(unsigned int scale, const UIntVector4 & vectorToScale)
{
	return UIntVector4(vectorToScale * scale);
}

unsigned int DotProduct(const UIntVector4 & a, const UIntVector4 & b)
{
	unsigned int result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	return result;
}

bool AreMostlyEqual(const UIntVector4 & a, const UIntVector4 & b, unsigned int epsilon)
{
	//UIntVector4 delta = a - b;
	//return fabs(delta.x) < epsilon && fabs(delta.y) < epsilon && fabs(delta.z) < epsilon;
	return a.IsMostlyEqualTo(b,epsilon);
}

std::ostream & operator<<(std::ostream & out, const UIntVector4 & pos)
{
	out << "UIntVector4( " << pos.x << " , " << pos.y << " , " << pos.z << " , " << pos.w << " )";
	return out;
}

unsigned int * UIntVector4::GetAsArray()
{
	return &x;
}

unsigned int UIntVector4::CalcLengthSquared() const
{
	return (x*x) + (y*y) + (z*z) + (w*w);
}

/*
unsigned int UIntVector4::CalcHeadingDegrees() const
{
	return Atan2Degrees(y, x);
}

unsigned int UIntVector4::CalcHeadingRadians() const
{
	return atan2(y,x);
}*/
