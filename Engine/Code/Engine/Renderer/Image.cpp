#include "Engine/Renderer/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

Image::Image()
	: m_filePath("")
	, m_dimensions(1,1)
	, m_bytesPerTexel(4)
	, m_texelBytes(nullptr)
{
	m_texelBytes = (unsigned char*)malloc(sizeof(unsigned char) * 4);//new unsigned char[4];
	memset(m_texelBytes, 255, m_bytesPerTexel);
}

Image::Image(const std::string& filePath)
	: m_filePath(filePath)
	, m_dimensions()
	, m_bytesPerTexel(0)
	, m_texelBytes(nullptr)
{
	// Load image data
	int width = 0;
	int height = 0;
	m_bytesPerTexel = 0;
	m_texelBytes = stbi_load(filePath.c_str(), &width, &height, &m_bytesPerTexel, 4);
	m_dimensions = IntVector2(width, height);
	GUARANTEE_OR_DIE(m_texelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", filePath.c_str()));
	GUARANTEE_OR_DIE(m_bytesPerTexel == 3 || m_bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", filePath.c_str(), m_bytesPerTexel));
}

Image::Image(const Rgba& color)
	: m_filePath("")
	, m_dimensions(1, 1)
	, m_bytesPerTexel(4)
	, m_texelBytes(nullptr)
{
	m_texelBytes = (unsigned char*)malloc(sizeof(unsigned char) * 4);//new unsigned char[4];
	memset(m_texelBytes, 255, m_bytesPerTexel);
	m_texelBytes[0] = color.red;
	m_texelBytes[1] = color.green;
	m_texelBytes[2] = color.blue;
	m_texelBytes[3] = color.alpha;
}

Image::Image(const Rgba& color, const IntVector2& dimensions)
	: m_filePath("")
	, m_dimensions(dimensions)
	, m_bytesPerTexel(4)
	, m_texelBytes(nullptr)
{
	size_t allocSize = sizeof(Rgba) * dimensions.x * dimensions.y;
	m_texelBytes = (unsigned char*)malloc(allocSize);//new unsigned char[4];
	memset(m_texelBytes, 255, allocSize);
	for (int y = 0; y < dimensions.y; y++)
		for (int x = 0; x < dimensions.x; x++)
		{
			int offset = (x + y * dimensions.x) * 4;
			m_texelBytes[offset + 0] = color.red;
			m_texelBytes[offset + 1] = color.green;
			m_texelBytes[offset + 2] = color.blue;
			m_texelBytes[offset + 3] = color.alpha;
		}
	
}

Image::Image(byte_t* data, int bytesPerTexel, const IntVector2& dimensions)
	: m_filePath("")
	, m_dimensions(dimensions)
	, m_bytesPerTexel(bytesPerTexel)
	, m_texelBytes(nullptr)
{
	size_t allocSize = m_bytesPerTexel * dimensions.x * dimensions.y;
	m_texelBytes = (unsigned char*)malloc(allocSize);
	memcpy(m_texelBytes, data, allocSize);
}

Image::~Image()
{
	if (m_texelBytes)
		free(m_texelBytes);
		//delete m_texelBytes;
}

Rgba Image::GetTexel(const IntVector2 & texelPosition) const
{
	int index = texelPosition.x + texelPosition.y * m_dimensions.x;
	return GetTexel(index);
}

Rgba Image::GetTexel(int index) const
{
	int bytesOffset = index * m_bytesPerTexel;

	Rgba color;

	color.red = m_texelBytes[bytesOffset];
	color.green = m_texelBytes[bytesOffset + 1];
	color.blue = m_texelBytes[bytesOffset + 2];
	if (m_bytesPerTexel == 4)
		color.alpha = m_texelBytes[bytesOffset + 3];

	return color;
}

void Image::SetTexel(const IntVector2 & texelPosition, const Rgba & color)
{
	int index = texelPosition.x + texelPosition.y * m_dimensions.x;
	m_texelBytes[index] = color.red;
	m_texelBytes[index+1] = color.red;
	m_texelBytes[index+2] = color.red;
	if(m_bytesPerTexel == 4)
		m_texelBytes[index+3] = color.red;
}

Image* Image::Crop(const AABB2& bounds)
{
	int left =		(int)MAX(bounds.mins.x,0);
	int right =		(int)MIN(bounds.maxs.x,m_dimensions.x);
	int top =		(int)MAX(bounds.mins.y,0);
	int bottom =	(int)MIN(bounds.maxs.y,m_dimensions.y);
	int width = right - left;
	int height = bottom - top;
	size_t allocSize = m_bytesPerTexel * width * height;
	byte_t* bytes = new byte_t[allocSize];
	for (int y = top; y < bottom; y++)
		for(int x = left; x < right; x++)
		{
			int destOffset = ((x - left) + (y - top) * width) * m_bytesPerTexel;
			int sourceOffset = (x + y * m_dimensions.x) * m_bytesPerTexel;
			for (int texIndex = 0; texIndex < m_bytesPerTexel; texIndex++)
			{
				bytes[destOffset + texIndex] = m_texelBytes[sourceOffset + texIndex];
			}
		}
	Image* croppedImg = new Image(bytes,m_bytesPerTexel,IntVector2(width,height));
	delete[] bytes;
	return croppedImg;
}

void Image::BgraSelf()
{
	for (int index = 0; index < m_dimensions.x * m_dimensions.y; index++)
	{
		int offset = index * m_bytesPerTexel;
		byte_t temp = m_texelBytes[offset + 0];
		m_texelBytes[offset + 0] = m_texelBytes[offset + 2];
		m_texelBytes[offset + 2] = temp;
	}
}
