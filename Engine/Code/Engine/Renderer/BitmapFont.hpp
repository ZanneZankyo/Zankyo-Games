#pragma once
#include <map>
#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"


#define DEFAULT_BITMAP_FONT BitmapFont(g_renderer->CreateOrGetTexture("Data/Fonts/ZankyoBitmapFont.png"), IntVector2(16,16))

class BitmapFont
{
public:
	const static std::string DEFAULT_FONT_NAME;
	const static float DEFAULT_BITMAP_FONT_ASPECT;

	std::string m_fontName;
	SpriteSheet m_sheet;
	std::map<char, int>* m_charMap;

public:
	BitmapFont();
	BitmapFont(const BitmapFont& copy);
	//explicit BitmapFont(const std::string& spriteSheetFilePath, Renderer* renderer, const IntVector2& layout, const std::map<char, int>* charMap = nullptr);
	explicit BitmapFont(const std::string& fontName, const SpriteSheet& sheet, const std::map<char, int>* charMap = nullptr);
	explicit BitmapFont(const std::string& fontName, const Texture* texture, const IntVector2& layout, const std::map<char, int>* charMap = nullptr);

	const Texture* GetTexture() const;
	AABB2 GetCharTextureCoords(char charUnicode) const;
};