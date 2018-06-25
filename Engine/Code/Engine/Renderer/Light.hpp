#pragma once
#include "Engine\Math\Vector4.hpp"
#include "Engine\Core\RGBA.hpp"
#include "Engine\Core\RGBAf.hpp"

struct PointLightAttributes
{
	Vector4 LIGHT_COLOR;
	Vector4 LIGHT_POSITION;
	Vector4 LIGHT_DIRECTION;
	Vector4 ATTENUATION;
	Vector4 SPEC_ATTENUATION;
};

class Light
{
public:
	PointLightAttributes m_attributes;
public:
	//alpha is intensity
	void SetColor(const Rgba& color)
	{
		//alpha is intensity
		RgbaF colorF(color);
		m_attributes.LIGHT_COLOR = Vector4(colorF.red, colorF.green, colorF.blue, color.alpha);
	}
	void SetPosition(const Vector3& position)
	{
		m_attributes.LIGHT_POSITION = Vector4(position);
	}
};