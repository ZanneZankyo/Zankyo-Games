#include "Engine/Core/RgbaF.hpp"
#include <math.h>
#include <stdlib.h>
#include "RGBA.hpp"

const RgbaF RgbaF::WHITE	= RgbaF(	1.f,	1.f,	1.f,	1.f);
const RgbaF RgbaF::BLACK	= RgbaF(	0.f,	0.f,	0.f,	1.f);
const RgbaF RgbaF::RED		= RgbaF(	1.f,	0.f,	0.f,	1.f);
const RgbaF RgbaF::GREEN	= RgbaF(	0.f,	1.f,	0.f,	1.f);
const RgbaF RgbaF::BLUE		= RgbaF(	0.f,	0.f,	1.f,	1.f);
const RgbaF RgbaF::YELLOW	= RgbaF(	1.f,	1.f,	0.f,	1.f);
const RgbaF RgbaF::PURPLE	= RgbaF(	1.f,	0.f,	1.f,	1.f);

RgbaF::RgbaF()
	: red(1.f)
	, green(1.f)
	, blue(1.f)
	, alpha(1.f)
{}

RgbaF::RgbaF(const RgbaF & copy)
	: red(copy.red)
	, green(copy.green)
	, blue(copy.blue)
	, alpha(copy.alpha)
{
}

RgbaF::RgbaF(float  r, float  g, float  b, float  a)
	: red(r)
	, green(g)
	, blue(b)
	, alpha(a)
{}

RgbaF::RgbaF(const char hexAdecimal[7], float  a)
	: alpha(a)
{
	char redStr[2] = { hexAdecimal[1], hexAdecimal[2] };
	char greenStr[2] = { hexAdecimal[3],hexAdecimal[4] };
	char blueStr[2] = { hexAdecimal[5],hexAdecimal[6] };
	float scale = 1 / 255.f;
	red = static_cast<float> (strtol(redStr, NULL, 16) * scale);
	green = static_cast<float> (strtol(greenStr, NULL, 16) * scale);
	blue = static_cast<float> (strtol(blueStr, NULL, 16) * scale);
}

RgbaF::RgbaF(unsigned int r8g8b8a8)
{
	const unsigned int redBit	= 0xff000000;
	const unsigned int greenBit = 0x00ff0000;
	const unsigned int blueBit	= 0x0000ff00;
	const unsigned int alphaBit = 0x000000ff;
	float scale = 1 / 255.f;
	red		= (float)((r8g8b8a8 & redBit)	>> 24) * scale;
	green	= (float)((r8g8b8a8 & greenBit)	>> 16) * scale;
	blue	= (float)((r8g8b8a8 & blueBit)	>> 8)  * scale;
	alpha	= (float) (r8g8b8a8 & alphaBit)		   * scale;
}

RgbaF::RgbaF(const Rgba& rgbaU)
{
	SetRgbaFromChar(rgbaU.red, rgbaU.green, rgbaU.blue, rgbaU.alpha);
}

void RgbaF::SetRgba(const RgbaF & copy)
{
	red = copy.red;
	green = copy.green;
	blue = copy.blue;
	alpha = copy.alpha;
}

void RgbaF::SetRgbaFromFloats(float  r, float  g, float  b, float  a)
{
	SetRgba(RgbaF(r, g, b, a));
}

void RgbaF::SetRgbaFromChar(unsigned char r, unsigned char g, unsigned char b, unsigned char a /*= 255*/)
{
	float scale = 1 / 255.f;
	SetRgba(RgbaF(r * scale, g * scale, b * scale, a * scale));
}

void RgbaF::SetRgbaFromHex(const char hexAdecimal[7], float  a)
{
	SetRgba(RgbaF(hexAdecimal, a));
}

void RgbaF::ScaleRGB(float rgbScale)
{
	red = red * rgbScale;
	green = green * rgbScale;
	blue = blue * rgbScale;
}

void RgbaF::ScaleAlpha(float alphaScale)
{
	alpha = alpha * alphaScale;
}

/*
void RgbaF::SetRgbaF(const char hexAdecimal[4], float  a)
{
	SetRgbaF(RgbaF(hexAdecimal, a));
}*/

void RgbaF::GetAsFloats(float & out_normalizedR, float & out_normalizedG, float & out_normalizedB, float & out_normalizedA)
{
	float scale = 1 / 255.f;
	out_normalizedR = red * scale;
	out_normalizedG = green * scale;
	out_normalizedB = blue * scale;
	out_normalizedA = alpha * scale;
}

const RgbaF & RgbaF::operator=(const RgbaF & copy)
{
	SetRgba(copy);
	return *this;
}

bool RgbaF::operator==(const RgbaF & RgbaFToEqual)
{
	return (red == RgbaFToEqual.red && green == RgbaFToEqual.green && blue == RgbaFToEqual.blue && alpha == RgbaFToEqual.alpha);
}

bool RgbaF::operator!=(const RgbaF & RgbaFToNotEqual)
{
	return !(*this == RgbaFToNotEqual);
}
