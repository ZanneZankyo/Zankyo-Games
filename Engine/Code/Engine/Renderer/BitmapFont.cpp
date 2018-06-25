#include "Engine/Renderer/BitmapFont.hpp"
const std::string BitmapFont::DEFAULT_FONT_NAME("ZankyoBitmapFont");
const float BitmapFont::DEFAULT_BITMAP_FONT_ASPECT = 10.f / 18.f; //Width / Height

BitmapFont::BitmapFont()
	: m_sheet()
	, m_charMap(nullptr)
	, m_fontName("")
{
}

BitmapFont::BitmapFont(const BitmapFont & copy)
	: m_sheet(copy.m_sheet)
	, m_charMap(nullptr)
	, m_fontName(copy.m_fontName)
{
	if (copy.m_charMap)
		m_charMap = new std::map<char, int>(*copy.m_charMap);
}

BitmapFont::BitmapFont(const std::string& fontName, const SpriteSheet & sheet, const std::map<char, int>* charMap)
	: m_sheet(sheet)
	, m_charMap(nullptr)
	, m_fontName(fontName)
{
	if (charMap)
		m_charMap = new std::map<char, int>(*m_charMap);
}

BitmapFont::BitmapFont(const std::string& fontName, const Texture* texture, const IntVector2 & layout, const std::map<char, int>* charMap)
	: m_sheet(texture, layout)
	, m_charMap(nullptr)
	, m_fontName(fontName)
{
	if (charMap)
		m_charMap = new std::map<char, int>(*charMap);
}


/*
BitmapFont::BitmapFont(const std::string& spriteSheetFilePath, Renderer* renderer, const IntVector2& layout, const std::map<char, int>* charMap)
	:m_sheet(spriteSheetFilePath, renderer, layout)
	, m_charMap(nullptr)
{
	if (charMap)
		m_charMap = new std::map<char, int>(*charMap);
}*/

const Texture * BitmapFont::GetTexture() const
{
	return m_sheet.m_texture;
}

AABB2 BitmapFont::GetCharTextureCoords(char charUnicode) const
{
	if(m_charMap)
		return m_sheet.GetTextCoordsForSpriteIndex((*m_charMap)[charUnicode]);
	else
		return m_sheet.GetTextCoordsForSpriteIndex(static_cast<int>(charUnicode));
}
