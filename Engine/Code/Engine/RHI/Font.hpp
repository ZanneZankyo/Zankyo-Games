#pragma once
#include <string>
#include <map>
#include "Engine/RHI/RHITypes.hpp"
#include <vector>

enum TextLineDirection
{
	TEXT_LINE_UP,
	TEXT_LINE_DOWN
};

enum TextAlignHorizontal
{
	TEXTALIGN_HORIZONTAL_LEFT,
	TEXTALIGN_HORIZONTAL_CENTER,
	TEXTALIGN_HORIZONTAL_RIGHT,
};

enum TextAlignVertical
{
	TEXTALIGN_VERTICAL_TOP,
	TEXTALIGN_VERTICAL_CENTER,
	TEXTALIGN_VERTICAL_BOTTOM,
};

struct FontTag
{
	int id;
	int x;
	int y;
	int width;
	int height;
	int xoffset;
	int yoffset;
	int xadvance;
	int page;
	int chnl;
	FontTag()
		: id(-1)
		, x(0), y(0), width(0), height(0), xoffset(0), yoffset(0)
		, xadvance(0), page(0), chnl(0)
	{}
};

class Font
{
public:
	std::string m_fontName;
	//std::map<int, FontTag> m_tags;
	std::vector<FontTag> m_tags;
	//std::map<std::pair<int, int>, int> m_kernings;
	std::vector<std::vector<int>> m_kernings;

	Texture2D* m_texture;

	int m_lineHeight;
	int m_base;
	int m_scaleW;
	int m_scaleH;
	int m_pages;

public:

	Font(RHIDevice* owner, const std::string& fileName);
	void LoadTexture(RHIDevice* owner);
	void LoadFromFntFile();

	static Font* CreateFontFromFile(RHIDevice* owner, const std::string& fileName);
	float CalculateLineWidth(const std::string& string, float scale);
	std::vector<float> Font::CalculateTextWidthes(const std::string& string, float scale);
	float CalcMaxLineWidth(const std::string& string, float scale);
	float CalculateTextHeight(const std::string& string, float scale);

	float CalcScaleByFontHeight(float textHeight);
	
};