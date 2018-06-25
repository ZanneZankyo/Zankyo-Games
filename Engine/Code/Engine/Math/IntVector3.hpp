#pragma once
#include <math.h>

class IntVector3
{
public:
	int x;
	int y;
	int z;

	const static IntVector3 ZERO;

public:
	IntVector3();
	IntVector3(const IntVector3& copy);
	explicit IntVector3(int a_x, int a_y, int a_z);
	explicit IntVector3(float a_x, float a_y, float a_z);

	void GetXY(int& outX, int& outY, int& outZ) const;
	void SetXY(int inX, int inY, int inZ);

	const IntVector3 operator + (const IntVector3& vectorToAdd) const;
	const IntVector3 operator - (const IntVector3& vectorToSubtract) const;
	const IntVector3 operator * (int scale) const;
	const IntVector3 operator * (const IntVector3& perAxisScaleFactors) const;
	const IntVector3 operator / (int inverseScale) const;
	const IntVector3 operator - () const;
	void operator *= (int scale);
	void operator *= (const IntVector3& perAxisScaleFactors);
	void operator /= (int inverseScale);
	void operator /= (const IntVector3& perAxisDivisors);
	void operator += (const IntVector3& vectorToAdd);
	void operator -= (const IntVector3& vectorToSubtract);

	bool operator == (const IntVector3& vectorToEqual);
	bool operator != (const IntVector3& vectorNotToEqual);

	
};

inline IntVector3::IntVector3()
	: x(0), y(0), z(0)
{}

inline IntVector3::IntVector3(const IntVector3 & copy)
	: x(copy.x), y(copy.y), z(copy.z)
{}

inline IntVector3::IntVector3(int a_x, int a_y, int a_z)
	: x(a_x), y(a_y), z(a_z)
{}

inline IntVector3::IntVector3(float a_x, float a_y, float a_z)
	: x(static_cast<int>(floor(a_x)))
	, y(static_cast<int>(floor(a_y)))
	, z(static_cast<int>(floor(a_z)))
{}

inline void IntVector3::GetXY(int & outX, int & outY, int& outZ) const
{
	outX = x;
	outY = y;
	outZ = z;
}

inline void IntVector3::SetXY(int inX, int inY, int inZ)
{
	x = inX;
	y = inY;
	z = inZ;
}

inline bool IntVector3::operator==(const IntVector3 & vectorToEqual)
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y) && (z == vectorToEqual.z);
}

inline bool IntVector3::operator!=(const IntVector3 & vectorNotToEqual)
{
	return !(*this == vectorNotToEqual);
}

inline const IntVector3 IntVector3::operator+(const IntVector3 & vectorToAdd) const
{
	return IntVector3(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
}

inline const IntVector3 IntVector3::operator-(const IntVector3 & vectorToSubtract) const
{
	return IntVector3(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
}

inline const IntVector3 IntVector3::operator*(int scale) const
{
	return IntVector3(x * scale, y * scale, z * scale);
}

inline const IntVector3 IntVector3::operator*(const IntVector3 & perAxisScaleFactors) const
{
	return IntVector3(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y, z * perAxisScaleFactors.z);
}

inline const IntVector3 IntVector3::operator/(int inverseScale) const
{
	if (inverseScale != 0)
	{
		int scale = 1 / inverseScale;
		return IntVector3(x * scale, y * scale, z * scale);
	}
	else
		return IntVector3::ZERO;
}

inline const IntVector3 IntVector3::operator-() const
{
	return IntVector3(-x, -y, -z);
}

inline void IntVector3::operator*=(int scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline void IntVector3::operator*=(const IntVector3 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
}

inline void IntVector3::operator/=(int inverseScale)
{
	if (inverseScale != 0)
	{
		int scale = 1 / inverseScale;
		x *= scale;
		y *= scale;
		z *= scale;
	}
}

inline void IntVector3::operator/=(const IntVector3 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	z /= perAxisDivisors.z;
}

inline void IntVector3::operator+=(const IntVector3 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}

inline void IntVector3::operator-=(const IntVector3 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
}
