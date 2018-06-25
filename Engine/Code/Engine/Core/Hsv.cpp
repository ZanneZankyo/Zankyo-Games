#include "Hsv.hpp"
#include "ErrorWarningAssert.hpp"

void Hsv::AddHue(float deltaHue)
{
	hue += deltaHue;
	hue = fmod(hue, 360.f);
	while (hue < 0)
		hue += 360.f;
}

Rgba Hsv::ToRgba() const
{
	return Rgba(ToRgbaF());
}

RgbaF Hsv::ToRgbaF() const
{
	int hi = (int)(hue / 60.f);
	float f = hue / 60.f - (float)hi;
	float p = value * (1 - saturation);
	float q = value * (1 - f * saturation);
	float t = value * (1 - (1 - f) * saturation);
	switch (hi)
	{
	case 0:
		return RgbaF(value, t, p);
	case 1:
		return RgbaF(q, value, p);
	case 2:
		return RgbaF(p, value, t);
	case 3:
		return RgbaF(p, q, value);
	case 4:
		return RgbaF(t, p, value);
	case 5:
		return RgbaF(value, p, q);
	default:
		ASSERT_OR_DIE(false, "Hsv::ToRgbaF(): wrong hi\n");
		return RgbaF();
	}
}
