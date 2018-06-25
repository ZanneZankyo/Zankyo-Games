#pragma once
#include "Engine\Renderer\Texture.hpp"
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\AABB2.hpp"
//#include "Renderer.hpp"
//#include <string>

class SpriteSheet
{
public:
	const Texture*	m_texture;
	IntVector2		m_tileLayout;

public:
	SpriteSheet();
	SpriteSheet(const Texture* texture, const IntVector2 layout);
	//SpriteSheet(const std::string& filePath, Renderer* renderer, const IntVector2 layout);

	AABB2 GetTextCoordsForSpriteIndex(int spriteIndex) const;
	AABB2 GetTextCoordsForSpriteIndex(const IntVector2& spriteIndex) const;
};