#include "Engine/Core/RGBA.hpp"
#include <math.h>
#include <stdlib.h>
#include "RGBAf.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "StringUtils.hpp"

const Rgba Rgba::WHITE	= Rgba(	255,	255,	255,	255);
const Rgba Rgba::BLACK	= Rgba(	0,		0,		0,		255);
const Rgba Rgba::RED	= Rgba(	255,	0,		0,		255);
const Rgba Rgba::GREEN	= Rgba(	0,		255,	0,		255);
const Rgba Rgba::BLUE	= Rgba(	0,		0,		255,	255);
const Rgba Rgba::YELLOW = Rgba(	255,	255,	0,		255);
const Rgba Rgba::PURPLE = Rgba(	255,	0,		255,	255);

Rgba::Rgba()
	: red(255)
	, green(255)
	, blue(255)
	, alpha(255)
{}

Rgba::Rgba(const Rgba & copy)
	: red(copy.red)
	, green(copy.green)
	, blue(copy.blue)
	, alpha(copy.alpha)
{
}

Rgba::Rgba(unsigned char  r, unsigned char  g, unsigned char  b, unsigned char  a)
	: red(r)
	, green(g)
	, blue(b)
	, alpha(a)
{}

/*
Rgba::Rgba(const char hexAdecimal[7], unsigned char  a)
	: alpha(a)
{
	char redStr[2] = { hexAdecimal[1], hexAdecimal[2] };
	char greenStr[2] = { hexAdecimal[3],hexAdecimal[4] };
	char blueStr[2] = { hexAdecimal[5],hexAdecimal[6] };
	red = static_cast<unsigned char> (strtol(redStr, NULL, 16));
	green = static_cast<unsigned char> (strtol(greenStr, NULL, 16));
	blue = static_cast<unsigned char> (strtol(blueStr, NULL, 16));
}*/

Rgba::Rgba(unsigned int r8g8b8a8)
{
	const unsigned int redBit	= 0xff000000;
	const unsigned int greenBit = 0x00ff0000;
	const unsigned int blueBit	= 0x0000ff00;
	const unsigned int alphaBit = 0x000000ff;

	red		= (unsigned char)((r8g8b8a8 & redBit)	>> 24);
	green	= (unsigned char)((r8g8b8a8 & greenBit)	>> 16);
	blue	= (unsigned char)((r8g8b8a8 & blueBit)	>> 8);
	alpha	= (unsigned char) (r8g8b8a8 & alphaBit);
}

Rgba::Rgba(const RgbaF& rgbaF)
{
	SetRgbaFromFloats(rgbaF.red, rgbaF.green, rgbaF.blue, rgbaF.alpha);
}

Rgba::Rgba(const std::string& colorString, unsigned char pAlpha /*= 255*/)
	: Rgba(255,255,255,pAlpha)
{
	std::string hex = colorString;
	if (colorString.empty())
		return;
	if (hex[0] == '#') // #rrggbbaa format
	{
		hex.erase(hex.begin());
		if (hex.size() == 3)
		{
			//convert rgba like #rgb
			char redStr[2] = { hex[0] };
			char greenStr[2] = { hex[1] };
			char blueStr[2] = { hex[2] };
			red = static_cast<unsigned char> (strtol(redStr, NULL, 16)) * 16;
			green = static_cast<unsigned char> (strtol(greenStr, NULL, 16)) * 16;
			blue = static_cast<unsigned char> (strtol(blueStr, NULL, 16)) * 16;
		}
		else if (hex.size() == 6)
		{
			//convert rgba like #rrggbb
			char redStr[2] = { hex[0], hex[1] };
			char greenStr[2] = { hex[2],hex[3] };
			char blueStr[2] = { hex[4],hex[5] };
			red = static_cast<unsigned char> (strtol(redStr, NULL, 16));
			green = static_cast<unsigned char> (strtol(greenStr, NULL, 16));
			blue = static_cast<unsigned char> (strtol(blueStr, NULL, 16));
		}
		else if (hex.size() == 8)
		{
			// convert rgba like #rrggbbaa
			char redStr[2] = { hex[0], hex[1] };
			char greenStr[2] = { hex[2],hex[3] };
			char blueStr[2] = { hex[4],hex[5] };
			char alphaStr[2] = { hex[6],hex[7] };
			red = static_cast<unsigned char> (strtol(redStr, NULL, 16));
			green = static_cast<unsigned char> (strtol(greenStr, NULL, 16));
			blue = static_cast<unsigned char> (strtol(blueStr, NULL, 16));
			alpha = static_cast<unsigned char> (strtol(alphaStr, NULL, 16));
		}
	}
	else // rrr,ggg,bbb,aaa format
	{
		std::vector<std::string> rgbaStrs = StringUtils::Split(colorString, ',');
		for (size_t strIndex = 0; strIndex < rgbaStrs.size(); strIndex++)
		{
			std::string rgbaStr(rgbaStrs[strIndex]);
			switch (strIndex)
			{
			case 0: red = (char)stoi(rgbaStr); break;
			case 1: green = (char)stoi(rgbaStr); break;
			case 2: blue = (char)stoi(rgbaStr); break;
			case 3: alpha = (char)stoi(rgbaStr); break;
			default: break;
			}
		}
	}
}

void Rgba::SetRgba(const Rgba & copy)
{
	red = copy.red;
	green = copy.green;
	blue = copy.blue;
	alpha = copy.alpha;
}

void Rgba::SetRgbaFromFloats(float normalizedR, float normalizedG, float normalizedB, float normalizedA /*= 1.f*/)
{
	SetRgba(Rgba((unsigned char)(normalizedR * 255.f), (unsigned char)(normalizedG * 255.f), (unsigned char)(normalizedB * 255.f), (unsigned char)(normalizedA * 255.f)));
}

void Rgba::SetRgbaFromChar(unsigned char  r, unsigned char  g, unsigned char  b, unsigned char  a)
{
	SetRgba(Rgba(r, g, b, a));
}

void Rgba::SetRgbaFromHex(const char hexAdecimal[7], unsigned char  a)
{
	SetRgba(Rgba(hexAdecimal, a));
}

void Rgba::ScaleRGB(float rgbScale)
{
	red = static_cast<unsigned char>(red * rgbScale);
	green = static_cast<unsigned char>(green * rgbScale);
	blue = static_cast<unsigned char>(blue * rgbScale);
}

void Rgba::ScaleAlpha(float alphaScale)
{
	alpha = static_cast<unsigned char>(alpha * alphaScale);
}

/*
void Rgba::SetRgba(const char hexAdecimal[4], unsigned char  a)
{
	SetRgba(Rgba(hexAdecimal, a));
}*/

void Rgba::GetAsFloats(float & out_normalizedR, float & out_normalizedG, float & out_normalizedB, float & out_normalizedA)
{
	float scale = 1 / 255.f;
	out_normalizedR = red * scale;
	out_normalizedG = green * scale;
	out_normalizedB = blue * scale;
	out_normalizedA = alpha * scale;
}

const Rgba & Rgba::operator=(const Rgba & copy)
{
	SetRgba(copy);
	return *this;
}

bool Rgba::operator==(const Rgba & RgbaToEqual)
{
	return (red == RgbaToEqual.red && green == RgbaToEqual.green && blue == RgbaToEqual.blue && alpha == RgbaToEqual.alpha);
}

bool Rgba::operator!=(const Rgba & RgbaToNotEqual)
{
	return !(*this == RgbaToNotEqual);
}

const Rgba Interpolate(const Rgba & start, const Rgba & end, float fractionToEnd)
{
	float fractionToStart = 1.f - fractionToEnd;
	float r = start.red * fractionToStart + end.red * fractionToEnd;
	float g = start.green * fractionToStart + end.green * fractionToEnd;
	float b = start.blue * fractionToStart + end.blue * fractionToEnd;
	float a = start.alpha * fractionToStart + end.alpha * fractionToEnd;
	return Rgba((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}