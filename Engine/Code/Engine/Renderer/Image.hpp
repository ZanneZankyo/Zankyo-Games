#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Core\RGBA.hpp"
#include <string>
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Math\AABB2.hpp"

class Image
{
public:
	Image();
	Image(const Rgba& color);
	Image(const Rgba& color, const IntVector2& dimensions);
	Image(const std::string& filePath);
	Image(byte_t* data, int bytesPerTexel, const IntVector2& dimensions);
	~Image();
	Rgba GetTexel(const IntVector2& texelPosition) const;
	Rgba GetTexel(int index) const;
	void SetTexel(const IntVector2& texelPosition, const Rgba& color);
	Image* Crop(const AABB2& bounds);
public:
	std::string		m_filePath;
	IntVector2		m_dimensions;
	unsigned char*  m_texelBytes;
	int				m_bytesPerTexel;
	
	
	void BgraSelf();
};
