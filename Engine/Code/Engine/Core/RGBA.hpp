#pragma once
#include <string>

class RgbaF;

class Rgba
{
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

	static const Rgba WHITE;
	static const Rgba BLACK;
	static const Rgba RED;
	static const Rgba GREEN;
	static const Rgba BLUE;
	static const Rgba YELLOW;
	static const Rgba PURPLE;

public:
	Rgba();
	Rgba(const Rgba& copy);
	explicit Rgba(unsigned char  r, unsigned char  g, unsigned char  b, unsigned char  a = 255);
	//explicit Rgba(const char hexAdecimal[7], unsigned char  alpha = 255);
	explicit Rgba(const std::string& hexAdecimal, unsigned char alpha = 255);
	explicit Rgba(unsigned int r8g8b8a8);
	explicit Rgba(const RgbaF& rgbaF);

	void SetRgba(const Rgba& copy);
	void SetRgbaFromFloats(float normalizedR, float normalizedG, float normalizedB, float normalizedA = 1.f);
	void SetRgbaFromChar(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255); // alpha still ranges from 0 to 1
	void SetRgbaFromHex(const char hexAdecimal[7], unsigned char a = 255);
	//void SetRgba(const char hexAdecimal[4], unsigned char  a = 1.f);
	void ScaleRGB(float rgbScale);
	void ScaleAlpha(float alphaScale);

	void GetAsFloats(float& out_normalizedR, float& out_normalizedG, float& out_normalizedB, float& out_normalizedA);

	const Rgba& operator = (const Rgba& copy);
	bool operator == (const Rgba& RgbaToEqual);
	bool operator != (const Rgba& RgbaToNotEqual);

	friend const Rgba Interpolate(const Rgba & start, const Rgba & end, float fractionToEnd);

};