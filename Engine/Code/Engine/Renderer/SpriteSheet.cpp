#include "SpriteSheet.hpp"


SpriteSheet::SpriteSheet()
	: m_texture(nullptr)
	, m_tileLayout(IntVector2::ZERO)
{
}


// SpriteSheet::SpriteSheet(const std::string & filePath, Renderer* renderer, const IntVector2 layout)
// 	: m_texture (*(renderer->CreateOrGetTexture(filePath)))
// 	, m_tileLayout(layout)
// {
// }

SpriteSheet::SpriteSheet(const Texture* texture, const IntVector2 layout)
	: m_texture(texture)
	, m_tileLayout(layout)
{
}

AABB2 SpriteSheet::GetTextCoordsForSpriteIndex(int spriteIndex) const
{
	int indexX = spriteIndex % m_tileLayout.x;
	int indexY = spriteIndex / m_tileLayout.x;
	return GetTextCoordsForSpriteIndex(IntVector2(indexX,indexY));
}

AABB2 SpriteSheet::GetTextCoordsForSpriteIndex(const IntVector2 & spriteIndex) const
{
	float percentPerTileX = 1.f / static_cast<float>(m_tileLayout.x);
	float percentPerTileY = 1.f / static_cast<float>(m_tileLayout.y);
	Vector2 mins = Vector2(percentPerTileX * static_cast<float>(spriteIndex.x), percentPerTileY * static_cast<float>(spriteIndex.y));
	Vector2 maxs = Vector2(percentPerTileX * static_cast<float>(spriteIndex.x+1), percentPerTileY * static_cast<float>(spriteIndex.y+1));
	return AABB2(mins, maxs);
}
