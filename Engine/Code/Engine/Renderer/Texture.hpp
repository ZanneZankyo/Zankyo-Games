#pragma once
#include <string>
#include "Engine/Math/IntVector2.hpp"

class Texture
{
	friend class Renderer;

public:
	Texture();
	~Texture();

	const Texture& operator = (const Texture& copy);

private:
	std::string			m_imageFilePath;
	unsigned int		m_textureID;
	IntVector2			m_texelDimensions;
};




