#pragma once
#include <math.h>
#include "AABB2.hpp"
#include "Vector2.hpp"

class Perlin
{
public:
	static float IntegerNoise(int n)
	{
		n = (n >> 13) ^ n;
		int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
		return 1.f - ((float)nn / 1073741824.f);
	}
	static float lerp(float a, float b, float w) { return a * (1 - w) + b * w; }
	static float bilinearInterpolation(
		const float bottomLeft, const float bottomRight, const float topLeft, const float topRight
		, const Vector2& weight)
	{
		float x = weight.x;
		float y = weight.y;
		return bottomLeft	* (1.f - x) * (1.f - y)
			 + bottomRight	* x			* (1 - y)
			 + topLeft		* (1 - x)	* y
			 + topRight		* x			* y;
	}
	static float CoherentNoise2(float x, float y)
	{
		int intX = (int)(floor(x));
		int intY = (int)(floor(y));
		float n00 = IntegerNoise(intX) + IntegerNoise(intY);
		float n10 = IntegerNoise(intX + 1) + IntegerNoise(intY);
		float n01 = IntegerNoise(intX) + IntegerNoise(intY + 1);
		float n11 = IntegerNoise(intX + 1) + IntegerNoise(intY + 1);
		Vector2 weight = Vector2(x - floor(x),y - floor(y));
		float noise = bilinearInterpolation(n00, n10, n01, n11, weight);
		return noise;
	}
};