#include "IntVector2.hpp"
#include "Vector2.hpp"
#include <sstream>

const IntVector2 IntVector2::ZERO(0, 0);


IntVector2::IntVector2(const Vector2 & copy)
	: x(static_cast<int>(copy.x))
	, y(static_cast<int>(copy.y))
{
}

int IntVector2::CalcCrossLength() const
{
	return abs(x) + abs(y);
}

IntVector2 IntVector2::GetCrossNormalized() const
{
	IntVector2 temp;
	if (x > 0)
		temp.x = 1;
	else if (x < 0)
		temp.x = -1;
	else
		temp.x = 0;
	if (y > 0)
		temp.y = 1;
	else if (y < 0)
		temp.y = -1;
	else
		temp.y = 0;
	return temp;
}

std::string IntVector2::ToString() const
{
	std::stringstream ss;
	ss << "(" << x << "," << y << ")";
	return ss.str();
}
