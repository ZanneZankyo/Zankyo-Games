#pragma once

class Rgba;

class RgbaF
{
public:
	float red;
	float green;
	float blue;
	float alpha;

	static const RgbaF WHITE;
	static const RgbaF BLACK;
	static const RgbaF RED;
	static const RgbaF GREEN;
	static const RgbaF BLUE;
	static const RgbaF YELLOW;
	static const RgbaF PURPLE;

public:
	RgbaF();
	RgbaF(const RgbaF& copy);
	explicit RgbaF(float  r, float  g, float  b, float  a = 1.f);
	explicit RgbaF(const char hexAdecimal[7], float  alpha = 1.f);
	explicit RgbaF(unsigned int r8g8b8a8);
	explicit RgbaF(const Rgba& rgbaU);

	void SetRgba(const RgbaF& copy);
	void SetRgbaFromFloats(float normalizedR, float normalizedG, float normalizedB, float normalizedA = 1.f);
	void SetRgbaFromChar(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 1.f); // alpha still ranges from 0 to 1
	void SetRgbaFromHex(const char hexAdecimal[7], float a = 1.f);
	void ScaleRGB(float rgbScale);
	void ScaleAlpha(float alphaScale);

	void GetAsFloats(float& out_normalizedR, float& out_normalizedG, float& out_normalizedB, float& out_normalizedA);

	const RgbaF& operator = (const RgbaF& copy);
	bool operator == (const RgbaF& RgbaFToEqual);
	bool operator != (const RgbaF& RgbaFToNotEqual);

};