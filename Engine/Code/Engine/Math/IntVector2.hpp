#pragma once
#include <string>

class IntVector2
{

	friend class Vector2;

public:
	int x;
	int y;

	const static IntVector2 ZERO;

public:
	IntVector2();
	IntVector2(const IntVector2& copy);
	IntVector2(const Vector2& copy);
	explicit IntVector2(int a_x, int a_y);

	void GetXY(int& outX, int& outY) const;
	void SetXY(int inX, int inY);
	int CalcCrossLength() const;
	IntVector2 GetCrossNormalized() const;

	std::string ToString() const;

	const IntVector2 operator + (const IntVector2& vectorToAdd) const;
	const IntVector2 operator - (const IntVector2& vectorToSubtract) const;
	const IntVector2 operator * (int scale) const;
	const IntVector2 operator * (const IntVector2& perAxisScaleFactors) const;
	const IntVector2 operator / (int inverseScale) const;
	const IntVector2 operator / (const IntVector2& perAxisInverseScaleFactors) const;
	const IntVector2 operator - () const;
	void operator *= (int scale);
	void operator *= (const IntVector2& perAxisScaleFactors);
	void operator /= (int inverseScale);
	void operator /= (const IntVector2& perAxisDivisors);
	void operator += (const IntVector2& vectorToAdd);
	void operator -= (const IntVector2& vectorToSubtract);

	bool operator == (const IntVector2& vectorToEqual) const;
	bool operator != (const IntVector2& vectorNotToEqual) const;

	friend bool operator < (const IntVector2& a, const IntVector2& b);
	friend int CalcDistanceSquared(const IntVector2& a, const IntVector2& b);
	
};


//---------

inline IntVector2::IntVector2()
	: x(0), y(0)
{
}

inline IntVector2::IntVector2(const IntVector2 & copy)
	: x(copy.x), y(copy.y)
{
}

inline IntVector2::IntVector2(int a_x, int a_y)
	: x(a_x), y(a_y)
{
}

inline void IntVector2::GetXY(int & outX, int & outY) const
{
	outX = x;
	outY = y;
}

inline void IntVector2::SetXY(int inX, int inY)
{
	x = inX;
	y = inY;
}

inline bool IntVector2::operator==(const IntVector2 & vectorToEqual) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y);
}

inline bool IntVector2::operator!=(const IntVector2 & vectorNotToEqual) const
{
	return !(*this == vectorNotToEqual);
}

inline const IntVector2 IntVector2::operator+(const IntVector2 & vectorToAdd) const
{
	return IntVector2(x + vectorToAdd.x, y + vectorToAdd.y);
}

inline const IntVector2 IntVector2::operator-(const IntVector2 & vectorToSubtract) const
{
	return IntVector2(x - vectorToSubtract.x, y - vectorToSubtract.y);
}

inline const IntVector2 IntVector2::operator*(int scale) const
{
	return IntVector2(x * scale, y * scale);
}

inline const IntVector2 IntVector2::operator*(const IntVector2 & perAxisScaleFactors) const
{
	return IntVector2(x * perAxisScaleFactors.x, y * perAxisScaleFactors.y);
}

inline const IntVector2 IntVector2::operator/(int inverseScale) const
{
	if (inverseScale != 0)
	{
		int scale = 1 / inverseScale;
		return IntVector2(x * scale, y * scale);
	}
	else
		return IntVector2::ZERO;
}

inline const IntVector2 IntVector2::operator/(const IntVector2 & perAxisInverseScaleFactors) const
{
	if (perAxisInverseScaleFactors.x != 0 && perAxisInverseScaleFactors.y != 0)
	{
		return IntVector2(x / perAxisInverseScaleFactors.x, y / perAxisInverseScaleFactors.y);
	}
	else
		return IntVector2::ZERO;
}

inline const IntVector2 IntVector2::operator-() const
{
	return IntVector2(-x, -y);
}

inline void IntVector2::operator*=(int scale)
{
	x *= scale;
	y *= scale;
}

inline void IntVector2::operator*=(const IntVector2 & perAxisScaleFactors)
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}

inline void IntVector2::operator/=(int inverseScale)
{
	if (inverseScale != 0)
	{
		int scale = 1 / inverseScale;
		x *= scale;
		y *= scale;
	}
}

inline void IntVector2::operator/=(const IntVector2 & perAxisDivisors)
{
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
}

inline void IntVector2::operator+=(const IntVector2 & vectorToAdd)
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}

inline void IntVector2::operator-=(const IntVector2 & vectorToSubtract)
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}

inline bool operator<(const IntVector2 & a, const IntVector2 & b)
{
	return a.x < b.x || (a.x == b.x && a.y < b.y);
}

inline int CalcDistanceSquared(const IntVector2 & a, const IntVector2 & b)
{
	IntVector2 delta = b - a;
	return delta.x * delta.x + delta.y * delta.y;
}
