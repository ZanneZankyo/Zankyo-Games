#include "Font.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\RHI\Texture2D.hpp"
#include "RHIDevice.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include <fstream>
#include <limits>
#include <stack>
#include <deque>
#include "..\Core\Profiler.hpp"

Font::Font(RHIDevice* owner, const std::string& fileName)
	: m_texture(nullptr)
	, m_kernings()
	, m_tags()
	, m_fontName()
	, m_lineHeight(0)
	, m_base(0)
	, m_scaleW(0)
	, m_scaleH(0)
	, m_pages(0)
{
	std::pair<std::string, std::string> pathInfo = StringUtils::SplitAtLastToken(fileName, '.');
	m_fontName = pathInfo.first;
	LoadTexture(owner);
	LoadFromFntFile();
}

void Font::LoadTexture(RHIDevice* owner)
{
	m_texture = new Texture2D(owner, "Data/Fonts/" + m_fontName + "_0.png");
}

void Font::LoadFromFntFile()
{
	std::string fontTagFileName = "Data/Fonts/" + m_fontName + ".fnt";
	std::ifstream in(fontTagFileName);
	ASSERT_OR_DIE(!in.bad(), "Loading \""+ m_fontName +".fnt\" file failed.");

	//StringUtils::IgnoreStreamLine(in);
	std::string line;

	std::getline(in, line);//ignore first line;

	//load common
	std::getline(in, line);
	std::vector<std::string> commonAttributes = StringUtils::Split(line, ' ');
	for (auto& attribute : commonAttributes)
	{
		std::pair<std::string, std::string> keyAndValue = StringUtils::SplitAtFirstToken(attribute, '=');
		if (keyAndValue.first == "lineHeight")
			m_lineHeight = stoi(keyAndValue.second);
		else if (keyAndValue.first == "base")
			m_base = stoi(keyAndValue.second);
		else if (keyAndValue.first == "scaleW")
			m_scaleW = stoi(keyAndValue.second);
		else if (keyAndValue.first == "scaleH")
			m_scaleH = stoi(keyAndValue.second);
		else if (keyAndValue.first == "pages")
			m_pages = stoi(keyAndValue.second);
	}

	std::getline(in, line);//ignore page line, assume there is only 1 texture in a font.

	//read char count
	std::getline(in, line);
	std::vector<std::string> charCountAttrs = StringUtils::Split(line, ' ');
	ASSERT_OR_DIE(charCountAttrs[0] == "chars", "Reading fnt file failed.");
	std::pair<std::string, std::string> charCountAttr = StringUtils::SplitAtFirstToken(charCountAttrs[1], '=');
	int totalCharCount = stoi(charCountAttr.second);

	//read attributes of each char
	for (int count = 0; count < totalCharCount; count++)
	{
		std::getline(in, line);
		std::vector<std::string> charAttributes = StringUtils::Split(line, ' ');
		FontTag tag;
		for (auto& attribute : charAttributes)
		{
			std::pair<std::string, std::string> keyAndValue = StringUtils::SplitAtFirstToken(attribute, '=');
			if (keyAndValue.first == "id")
				tag.id = stoi(keyAndValue.second);
			else if (keyAndValue.first == "x")
				tag.x = stoi(keyAndValue.second);
			else if (keyAndValue.first == "y")
				tag.y = stoi(keyAndValue.second);
			else if (keyAndValue.first == "width")
				tag.width = stoi(keyAndValue.second);
			else if (keyAndValue.first == "height")
				tag.height = stoi(keyAndValue.second);
			else if (keyAndValue.first == "xoffset")
				tag.xoffset = stoi(keyAndValue.second);
			else if (keyAndValue.first == "yoffset")
				tag.yoffset = stoi(keyAndValue.second);
			else if (keyAndValue.first == "xadvance")
				tag.xadvance = stoi(keyAndValue.second);
			else if (keyAndValue.first == "page")
				tag.page = stoi(keyAndValue.second);
			else if (keyAndValue.first == "chnl")
				tag.chnl = stoi(keyAndValue.second);
		}
		if (tag.id >= (int)m_tags.size())
			m_tags.resize(tag.id + 1);
		m_tags[tag.id] = tag;
	}

	//read kerning count
	std::getline(in, line);
	std::vector<std::string> kerningCountAttrs = StringUtils::Split(line, ' ');
	if (kerningCountAttrs.empty() || kerningCountAttrs[0] != "kernings")
		return;
	//ASSERT_OR_DIE(kerningCountAttrs[0] == "kernings", "Reading fnt file failed.");
	std::pair<std::string, std::string> kerningCountAttr = StringUtils::SplitAtFirstToken(kerningCountAttrs[1], '=');
	int totalKerningCount = stoi(kerningCountAttr.second);

	//read attributes of each kerning
	for (int count = 0; count < totalKerningCount; count++)
	{
		std::getline(in, line);
		std::vector<std::string> kerningAttributes = StringUtils::Split(line, ' ');
		std::pair<int, int> kerningIndex;
		int amount = 0;
		for (auto& attribute : kerningAttributes)
		{
			std::pair<std::string, std::string> keyAndValue = StringUtils::SplitAtFirstToken(attribute, '=');
			if (keyAndValue.first == "first")
				kerningIndex.first = stoi(keyAndValue.second);
			else if (keyAndValue.first == "second")
				kerningIndex.second = stoi(keyAndValue.second);
			else if (keyAndValue.first == "amount")
				amount = stoi(keyAndValue.second);
		}
		if (kerningIndex.first >= (int)m_kernings.size())
			m_kernings.resize(kerningIndex.first + 1);
		if(kerningIndex.second >= (int)m_kernings[kerningIndex.first].size())
			m_kernings[kerningIndex.first].resize(kerningIndex.second + 1, 0);
		m_kernings[kerningIndex.first][kerningIndex.second] = amount;
	}

}

Font* Font::CreateFontFromFile(RHIDevice* owner, const std::string& fileName)
{
	return new Font(owner, fileName);
}

float Font::CalculateLineWidth(const std::string & string, float scale)
{
	float width = 0;

	for (size_t stringIndex = 0; stringIndex < string.size(); stringIndex++)
	{
		char printChar = string[stringIndex];

		if (printChar == '\n')
		{
			return width;
		}

		//std::map<int, FontTag>::iterator tagFound = m_tags.find(printChar);

		//ASSERT_OR_DIE(tagFound != font->m_tags.end(), "undefined font tags of char " + printChar);
		//if (tagFound == m_tags.end())
		//	continue;
		if(printChar >= (int)m_tags.size())
			continue;

		//FontTag tag = tagFound->second;
		FontTag& tag = m_tags[printChar];

		width += tag.xadvance * scale;
	}
	return width;
}

std::vector<float> Font::CalculateTextWidthes(const std::string& string, float scale)
{
	PROFILE_FUNCTION;
	std::deque<float> widthes;
	widthes.push_back(0);

	for (size_t stringIndex = 0; stringIndex < string.size(); stringIndex++)
	{
		char printChar = string[stringIndex];

		if (printChar == '\n')
		{
			widthes.push_back(0);
			continue;
		}

		//std::map<int, FontTag>::iterator tagFound = m_tags.find(printChar);

		//ASSERT_OR_DIE(tagFound != font->m_tags.end(), "undefined font tags of char " + printChar);
		//if (tagFound == m_tags.end())
		//	continue;
		if (printChar >= (int)m_tags.size())
			continue;

		//FontTag tag = tagFound->second;
		FontTag& tag = m_tags[printChar];

		widthes.back() += tag.xadvance * scale;


	}

	

	std::vector<float> outs;
	outs.reserve(widthes.size());

	while (!widthes.empty())
	{
		outs.push_back(widthes.front());
		widthes.pop_front();
	}
	return outs;
}

float Font::CalcMaxLineWidth(const std::string& string, float scale)
{
	std::vector<float> widthes = CalculateTextWidthes(string, scale);
	float maxWidth = 0.f;
	for (auto& width : widthes)
		if (width > maxWidth)
			maxWidth = width;
	return maxWidth;
}

float Font::CalculateTextHeight(const std::string & string, float scale)
{
	int lineSize = StringUtils::GetLineSize(string);
	return lineSize * m_lineHeight * scale;
}

float Font::CalcScaleByFontHeight(float textHeight)
{
	return textHeight / m_lineHeight;
}
