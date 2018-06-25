#pragma once
#include "RGBA.hpp"
#include "RGBAf.hpp"

class Hsv
{
public:
	float hue;
	float saturation; 
	float value;
	Hsv(float h = 0, float s = 0, float v = 0) : hue(fmod(h, 360.f)), saturation(s), value(v) {}
	Hsv(const Hsv& copy) : hue(copy.hue), saturation(copy.saturation), value(copy.value) {}
	void AddHue(float deltaHue);
	Rgba ToRgba() const;
	RgbaF ToRgbaF() const;
};