#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Texture::Texture()
	: m_imageFilePath( "UNKNOWN IMAGE FILE" )
	, m_textureID( 0 )
{

}


//-----------------------------------------------------------------------------------------------
Texture::~Texture()
{

}

const Texture & Texture::operator=(const Texture & copy)
{
	m_textureID			= copy.m_textureID;
	m_imageFilePath		= copy.m_imageFilePath;
	m_texelDimensions	= copy.m_texelDimensions;
	return *this;
}

