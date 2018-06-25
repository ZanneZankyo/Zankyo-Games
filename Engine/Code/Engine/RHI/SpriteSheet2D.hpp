#pragma once
#include "Engine\Math\IntVector2.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Texture2D.hpp"
//#include "Renderer.hpp"
//#include <string>

class SpriteSheet2D
{
public:
	const Texture2D*	m_texture;
	IntVector2		m_tileLayout;

public:
	SpriteSheet2D();
	SpriteSheet2D(const Texture2D* texture, const IntVector2 layout);
	//SpriteSheet(const std::string& filePath, Renderer* renderer, const IntVector2 layout);

	AABB2 GetTextCoordsForSpriteIndex(int spriteIndex) const;
	AABB2 GetTextCoordsForSpriteIndex(const IntVector2& spriteIndex) const;
};