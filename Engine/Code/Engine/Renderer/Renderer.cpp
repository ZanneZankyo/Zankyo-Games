#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include "Engine/Core/Win32Include.hpp"
#include <gl/gl.h>					// #ToDo: Remove all references to OpenGL
#include <gl/glu.h>
#include "OpenGLExtensions.hpp"
#include <complex>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/StringUtils.hpp"
#include "ThirdParty/stb/stb_image.h"
//#include <stddef.h>
#include "ThirdParty/OpenGL/glext.h"
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" ) // Link in the Glu32.lib static library

const float RGBA_SCALE = 1.f / 255.f;
const std::string Renderer::DEFAULT_TEXTURE_NAME = "DEFAULT!";

extern const int PRIMITIVE_POINTS = 1;
extern const int PRIMITIVE_LINES = 2;
extern const int PRIMITIVE_LINE_LOOP = 3;
extern const int PRIMITIVE_TRIANGLES = 4;
extern const int PRIMITIVE_QUADS = 5;

Renderer::Renderer()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	GUARANTEE_OR_DIE(glGenBuffers && glBindBuffer && glBufferData && glGenerateMipmap, "Modern OpenGL Functions not supported on this computer");

	CreateDefaultTexture();
}

void Renderer::Finish()
{
	glFinish();
}

//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ClearScreen(Rgba color)
{
	glClearColor(color.red * RGBA_SCALE, color.green * RGBA_SCALE, color.blue * RGBA_SCALE, color.alpha * RGBA_SCALE);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::EnableDepthTesting(bool isNowDepthTesting)
{
	if (isNowDepthTesting)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Renderer::CreateDefaultTexture()
{
	unsigned char defaultTexture[4] = { 255,255,255,255 };
	unsigned int openGLTextureID = CreateOpenGLTexture(defaultTexture, 1, 1, 4);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = DEFAULT_TEXTURE_NAME;
	texture->m_texelDimensions.SetXY(1, 1);

	m_alreadyLoadedTextures.push_back(texture);
}

void Renderer::BindTexture(const Texture * texture)
{
	glEnable(GL_TEXTURE_2D);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture->m_textureID);
	else
		glBindTexture(GL_TEXTURE_2D, GetTexture(DEFAULT_TEXTURE_NAME)->m_textureID);
}

void Renderer::EnableBackFaceCulling(bool isNowBackFaceCulling)
{
	if (isNowBackFaceCulling)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
// 	glFrontFace(GL_CCW);
// 	glCullFace(GL_BACK);
	
}

void Renderer::Translate2D(const Vector2 & translation)
{
	glTranslatef(translation.x, translation.y, 0.f);
}

void Renderer::Rotate2D(float degrees)
{
	glRotatef(degrees, 0.f, 0.f, 1.f);
}

void Renderer::Scale2D(const Vector2 & scale)
{
	glScalef(scale.x, scale.y, 1.f);
}

void Renderer::DrawPoint2(const Vector2 & pos, const Rgba & color)
{
	glBegin(GL_POINTS);
	glColor4ub(color.red, color.green, color.blue, color.alpha);
	glVertex2f(pos.x, pos.y);
	glEnd();
}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawLine(const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float thickness)
{
	if (thickness == -1.f) // default drawing(thickness not indicated)
	{
		glBegin(GL_LINES);
		{
			glColor4ub(startColor.red, startColor.green, startColor.blue, startColor.alpha);
			glVertex2f(start.x, start.y);
			glColor4ub(endColor.red, endColor.green, endColor.blue, endColor.alpha);
			glVertex2f(end.x, end.y);
		}
		glEnd();
	}
	else
	{
		Vertex2 startLeft = Vertex2(start,startColor), startRight = Vertex2(start, startColor);
		Vertex2 endRight = Vertex2(end,endColor), endLeft = Vertex2(end, endColor);

		Vector2 translation = Vector2(end - start);

		translation.SetLength(thickness * 0.5f);
		translation.Rotate90Degrees();
		startLeft.position += translation;
		startRight.position -= translation;
		endLeft.position += translation;
		endRight.position -= translation;

		Vertex2 vertexes[4] = { startLeft ,startRight, endRight, endLeft };

		//DrawLineStrip(vertexes, 4);
		glBegin(GL_TRIANGLE_FAN);
		for (int vertexIndex = 0; vertexIndex < 4; vertexIndex++) {
			Vector2 pos = vertexes[vertexIndex].position;
			Rgba color = vertexes[vertexIndex].color;
			glColor4ub(color.red, color.blue, color.green, color.alpha);
			glVertex2f(pos.x, pos.y);
		}
		glColor4ub(vertexes[0].color.red, vertexes[0].color.blue, vertexes[0].color.green, vertexes[0].color.alpha);
		glVertex2f(vertexes[0].position.x, vertexes[0].position.y);
		glEnd();
	}
}

void Renderer::DrawLine(const Ray2 & line, const Rgba & color, float thickness)
{
	DrawLine(line.start, line.end, color, color, thickness);
}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawRagularPolygonOutline(float x, float y, float sides, float radius, float offsetDegree, Rgba color)
{
	glColor4ub(color.red, color.green, color.blue, color.alpha); 
	float degreePerSide = 360.f / sides;
	glBegin(GL_LINE_LOOP);
	for (float degree = 0.f; degree < 360.f; degree += degreePerSide)
	{
		float rad = ConvertDegreesToRadians(degree + offsetDegree);
		glVertex2f(x + radius * cos(rad), y + radius * sin(rad));
	}
	glEnd();
}

void Renderer::DrawCircle(float radius, float sides, Rgba color)
{
	DrawRagularPolygonOutline(0.f, 0.f, sides, radius, 0.f, color);
}

//-----------------------------------------------------------------------------------------------
void Renderer::DrawCircle(float x, float y, float radius, float sides, Rgba color)
{
	DrawRagularPolygonOutline(x, y, sides, radius, 0.f, color);
}

void Renderer::DrawShape(const Vector2& position, const Vector2* points, int numberOfPoints, float offsetDegree, Rgba color)
{
	glColor4ub(color.red, color.green, color.blue, color.alpha);
	
	//glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, 0.f);
	glRotatef(offsetDegree, 0.f, 0.f, 1.f);

	glBegin(GL_LINE_LOOP);
	for (int i=0;i<numberOfPoints;i++)
	{
		Vector2 point = points[i];
		//point.RotateDegrees(offsetDegree);
		//point += position;
		glVertex2f(point.x, point.y);
	}
	glEnd();

	glPopMatrix();
}

void Renderer::DrawShape(const Vector2& position, const Vertex2* vertexes, int numberOfVertexes, float offsetDegree /*= 0.f*/)
{
	

	//glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, 0.f);
	glRotatef(offsetDegree, 0.f, 0.f, 1.f);

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numberOfVertexes; i++)
	{
		Vertex2 vertex = vertexes[i];
		Rgba color = vertex.color;
		Vector2 pos = vertex.position;
		glColor4ub(color.red, color.green, color.blue, color.alpha);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();

	glPopMatrix();
}

void Renderer::DrawDisc2(float radius, const Rgba & color, float sides)
{
	glColor4ub(color.red, color.green, color.blue, color.alpha);

	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(0, 0);
	float addDegrees = 360.f / sides;
	for (float degrees = 0; degrees <= 360.f; degrees += addDegrees)
	{
		glVertex2f(radius * CosDegrees(degrees), radius * SinDegrees(degrees));
	}
	glEnd();

}

void Renderer::DrawShape(const Vertex2* vertexes, int numberOfVertexes)
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numberOfVertexes; i++)
	{
		Vertex2 vertex = vertexes[i];
		Rgba color = vertex.color;
		Vector2 pos = vertex.position;
		glColor4ub(color.red, color.green, color.blue, color.alpha);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();
}

void Renderer::DrawDisc2(const Vector2 & position, float radius, const Rgba & color, float sides)
{
	glPushMatrix();
	glTranslatef(position.x, position.y, 0.f);

	DrawDisc2(radius, color, sides);

	glPopMatrix();
}

void Renderer::DrawLineStrip(const Vertex2 * vertexArray, int numOfVertexes)
{
	glBegin(GL_LINE_STRIP);
	for (int vertexIndex = 0; vertexIndex < numOfVertexes; vertexIndex++)
	{
		const Vertex2& vertex = vertexArray[vertexIndex];
		const Rgba& color = vertex.color;
		const Vector2& pos = vertex.position;
		glColor4ub(color.red, color.blue, color.green, color.alpha);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();
}

void Renderer::DrawAABB2(const AABB2 & bounds, const Rgba & color)
{
	glColor4ub(color.red, color.green, color.blue, color.alpha);

	glBegin(GL_TRIANGLE_FAN);
		
		glVertex2f(bounds.mins.x, bounds.mins.y);
		glVertex2f(bounds.maxs.x, bounds.mins.y);
		glVertex2f(bounds.maxs.x, bounds.maxs.y);
		glVertex2f(bounds.mins.x, bounds.maxs.y);

	glEnd();
}

void Renderer::DrawCapsule2(const Capsule2 & capsule, const Rgba & color)
{
	DrawDisc2(capsule.segment.start, capsule.radius, color);
	DrawDisc2(capsule.segment.end, capsule.radius, color);
	DrawLine(capsule.segment.start, capsule.segment.end, color, color, capsule.radius * 2.f);
}

void Renderer::SetOrthoProjection(const Vector2 & bottomLeft, const Vector2 & topRight)
{
	glLoadIdentity();
	glOrtho(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f);
}

void Renderer::SetOrthoProjection(float bottomLeftX, float bottomLeftY, float topRightX, float topRightY)
{
	SetOrthoProjection(Vector2(bottomLeftX, bottomLeftY), Vector2(topRightX, topRightY));
}

void Renderer::SetOrthoProjection(const AABB2 & bound)
{
	SetOrthoProjection(bound.mins, bound.maxs);
}

void Renderer::SetOrthoCentered(const Vector2 & center, const Vector2 & size)
{
	SetOrthoProjection(center - size * 0.5f, center + size * 0.5f);
}

void Renderer::SetOrthoCentered(float centerX, float centerY, float sizeX, float sizeY)
{
	SetOrthoCentered(Vector2(centerX, centerY), Vector2(sizeX, sizeY));
}

void Renderer::RotateAroundPosition2D(const Vector2& position, float Degrees)
{
	Translate2D(position);
	Rotate2D(Degrees);
	Translate2D(position * -1.f);
}

Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	Texture* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
}


//-----------------------------------------------------------------------------------------------
/*
void Renderer::DrawTexturedAABB2(const AABB2& bounds, const Texture& texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.f, 1.f);
		glVertex2f(bounds.mins.x, bounds.mins.y);

		glTexCoord2f(1.f, 1.f);
		glVertex2f(bounds.maxs.x, bounds.mins.y);

		glTexCoord2f(1.f, 0.f);
		glVertex2f(bounds.maxs.x, bounds.maxs.y);

		glTexCoord2f(0.f, 0.f);
		glVertex2f(bounds.mins.x, bounds.maxs.y);
	}
	glEnd();
}*/

void Renderer::DrawTexturedAABB2(const AABB2 & bounds, const Texture & texture, const AABB2 & textureCoods, const Rgba& color)
{
	Vector2 mins = textureCoods.mins;
	Vector2 maxs = textureCoods.maxs;
	Vertex3 vertexes[] =
	{
		Vertex3(Vector3(bounds.mins.x, bounds.mins.y, 0.f),color,Vector2(mins.x, maxs.y)),
		Vertex3(Vector3(bounds.maxs.x, bounds.mins.y, 0.f),color,Vector2(maxs.x, maxs.y)),
		Vertex3(Vector3(bounds.maxs.x, bounds.maxs.y, 0.f),color,Vector2(maxs.x, mins.y)),
		Vertex3(Vector3(bounds.mins.x, bounds.maxs.y, 0.f),color,Vector2(mins.x, mins.y)),
	};
	int numOfVertexes = sizeof(vertexes) / sizeof(Vertex3);
	DrawQuads3D(vertexes, numOfVertexes, &texture);
	/*glColor4ub(color.red, color.green, color.blue, color.alpha);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_textureID);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(mins.x, maxs.y);
		glVertex2f(bounds.mins.x, bounds.mins.y);

		glTexCoord2f(maxs.x, maxs.y);
		glVertex2f(bounds.maxs.x, bounds.mins.y);

		glTexCoord2f(maxs.x, mins.y);
		glVertex2f(bounds.maxs.x, bounds.maxs.y);

		glTexCoord2f(mins.x, mins.y);
		glVertex2f(bounds.mins.x, bounds.maxs.y);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);*/
}

void Renderer::DrawTexturedAABB2(const AABB2 & bounds, const Texture * texture, const AABB2 & textureCoods, const Rgba & color)
{
	DrawTexturedAABB2(bounds, *texture, textureCoods, color);
}

void Renderer::DrawText2(const Vector2 & startTopLeft, const std::string& text, float drawWidth, const BitmapFont & font, const Rgba& color, float fontAspect, const std::string& align)//#TODO add align feature
{
	UNUSED(align);
	float drawHeight = drawWidth / fontAspect;
	Vector2 drawPos = startTopLeft - Vector2(0.f, drawHeight);
	AABB2 drawAABB = AABB2(drawPos, drawPos + Vector2(drawWidth, drawHeight));
	for (unsigned int textIndex = 0; textIndex < text.size(); textIndex++)
	{
		char currentChar = text[textIndex];
		if (currentChar == '\n')
		{
			drawPos -= Vector2(0.f, drawHeight);
			drawAABB = AABB2(drawPos, drawPos + Vector2(drawWidth, drawHeight));
			continue;
		}
		AABB2 charTextureCood = font.GetCharTextureCoords(currentChar);
		DrawTexturedAABB2(drawAABB, *font.GetTexture(), charTextureCood, color);
		drawAABB += Vector2(drawWidth, 0.f);
	}

	//#ToDo align text
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(const std::string& imageFilePath)
{
	// Load image data
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, Stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, Stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = imageFilePath;
	texture->m_texelDimensions.SetXY(width, height);

	m_alreadyLoadedTextures.push_back(texture);
	return texture;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex)
	{
		Texture* texture = m_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	// Create a texture ID (or "name" as OpenGL calls it) for this new texture
	unsigned int openGLTextureID = 0xFFFFFFFF;
	glGenTextures(1, &openGLTextureID);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture(GL_TEXTURE_2D, openGLTextureID);

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

																  // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (bytesPerTexel == 3)
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageTexelBytes);	// Location of the actual pixel data bytes/buffer
	glGenerateMipmap(GL_TEXTURE_2D);
	return openGLTextureID;
}

BitmapFont * Renderer::CreateOrGetBitFont(const std::string & fontName)
{
	// Try to find that texture from those already loaded
	BitmapFont* bitFont = GetBitFont(fontName);
	if (bitFont)
		return bitFont;

	return CreateBitFontFromFile(fontName);
}

BitmapFont * Renderer::CreateBitFontFromFile(const std::string & fontName)
{
	Texture* texture = CreateOrGetTexture("Data/Fonts/" + fontName + ".png");
	if(!texture)
		texture = CreateOrGetTexture("Data/Fonts/" + BitmapFont::DEFAULT_FONT_NAME + ".png");
	BitmapFont* newFont = new BitmapFont(fontName, SpriteSheet(texture, IntVector2(16, 16)));
	m_alreadyLoadedBitmapFonts.push_back(newFont);
	return newFont;
}

BitmapFont * Renderer::GetBitFont(const std::string & fontName)
{
	for (int fontIndex = 0; fontIndex < (int)m_alreadyLoadedBitmapFonts.size(); ++fontIndex)
	{
		BitmapFont* bitFont = m_alreadyLoadedBitmapFonts[fontIndex];
		if (fontName == bitFont->m_fontName)
			return bitFont;
	}

	return nullptr;
}

void Renderer::SetColor(Rgba color)
{
	glColor4ub(color.red, color.green, color.blue, color.alpha);
}

void Renderer::SetAdditiveBlending()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Renderer::SetSubtractiveBlending()
{
	glBlendFunc(GL_FUNC_SUBTRACT, GL_ONE);
}

void Renderer::SetAlphaBlending()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance)
{
	glLoadIdentity();
	gluPerspective(fovVerticalDegrees, aspectRatio, nearDistance, farDistance);
}

void Renderer::Translate3D(const Vector3 & translation)
{
	Translate3D(translation.x, translation.y, translation.z);
}

void Renderer::Translate3D(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::Rotate3D(float degrees, float rotateAxisX, float rotateAxisY, float rotateAxisZ)
{
	glRotatef(degrees, rotateAxisX, rotateAxisY, rotateAxisZ);
}

void Renderer::Rotate3D(float degrees, Vector3 basis)
{
	Rotate3D(degrees, basis.x, basis.y, basis.z);
}

void Renderer::Scale3D(float x, float y, float z)
{
	glScalef(x, y, z);
}

void Renderer::Scale3D(const Vector3 & scales)
{
	Scale3D(scales.x, scales.y, scales.z);
}

void Renderer::Scale3D(float uniformScale)
{
	Scale3D(uniformScale, uniformScale, uniformScale);
}

void Renderer::GetCurrentMatrix(float* matrix)
{
	return glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}

void Renderer::LoadMatrix(const Matrix4 & matrix)
{
	glLoadMatrixf(matrix.GetAsFloatArray());
}

void Renderer::MultplyMatrix(const Matrix4 & matrix)
{
	glMultMatrixf(matrix.GetAsFloatArray());
}

void Renderer::LookAt(const Vector3 & eyePosition, const Vector3 & lookPosition, const Vector3 & upDirection)
{
	gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z, lookPosition.x, lookPosition.y, lookPosition.z, upDirection.x, upDirection.y, upDirection.z);
}

void Renderer::DrawAxes(float lengthX, float lengthY, float lengthZ)
{
	Vertex3 vertexes[] =
	{
		Vertex3(Vector3(0.f, 0.f, 0.f),Rgba(255, 0, 0, 255)),
		Vertex3(Vector3(lengthX, 0.f, 0.f),Rgba(255, 0, 0, 255)),
		Vertex3(Vector3(0.f, 0.f, 0.f),Rgba(0, 255, 0, 255)),
		Vertex3(Vector3(0.f, lengthY, 0.f),Rgba(0, 255, 0, 255)),
		Vertex3(Vector3(0.f, 0.f, 0.f),Rgba(0, 0, 255, 255)),
		Vertex3(Vector3(0.f, 0.f, lengthZ),Rgba(0, 0, 255, 255)),
	};
	int numOfVertexes = sizeof(vertexes) / sizeof(Vertex3);
	DrawLines3D(vertexes, numOfVertexes);
	/*glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glColor4f(1.f, 0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(lengthX, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, lengthY, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, lengthZ);

	glEnd();*/
}

void Renderer::DrawPoints3D(const Vertex3 * vertexes, int numOfVertexes)
{
// 	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
// 	glEnable(GL_POINT_SMOOTH);
// 	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPointSize(5);
	DrawVertexArray(vertexes, numOfVertexes, nullptr, PRIMITIVE_POINTS);
// 	glDisable(GL_PROGRAM_POINT_SIZE_EXT);
// 	glDisable(GL_POINT_SMOOTH);
// 	glDisable(GL_BLEND);
	/*BindTexture(nullptr);

	//glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	constexpr int stride = sizeof(Vertex3);

	glVertexPointer(3, GL_FLOAT, stride, &vertexes[0].position);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexes[0].color);

	glDrawArrays(GL_POINTS, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//glDisable(GL_POINT_SMOOTH);
	//glDisable(GL_BLEND);*/

}

void Renderer::DrawQuads3D(const Vertex3 * vertexes, int numOfVertexes, const Texture * texture)
{
	DrawVertexArray(vertexes, numOfVertexes, texture, PRIMITIVE_QUADS);
	/*BindTexture(texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3);
	
	glVertexPointer(	3, GL_FLOAT,			stride, &vertexes[0].position);
	glColorPointer(		4, GL_UNSIGNED_BYTE,	stride, &vertexes[0].color);
	glTexCoordPointer(	2, GL_FLOAT,			stride, &vertexes[0].texCoords);

	glDrawArrays(GL_QUADS, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
}

void Renderer::DrawVertexArray(const Vertex3 * vertexes, int numOfVertexes, const Texture * texture, int primitiveType)
{
	BindTexture(texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	constexpr int stride = sizeof(Vertex3);

	glVertexPointer(3, GL_FLOAT, stride, &vertexes[0].position);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexes[0].color);
	glTexCoordPointer(2, GL_FLOAT, stride, &vertexes[0].texCoords);

	int drawShape;
	switch (primitiveType)
	{
	case PRIMITIVE_POINTS: 
		drawShape = GL_POINTS; break;
	case PRIMITIVE_LINES:
		drawShape = GL_LINES; break;
	case PRIMITIVE_LINE_LOOP:
		drawShape = GL_LINE_LOOP; break;
	case PRIMITIVE_TRIANGLES:
		drawShape = GL_TRIANGLE_LIST_SUN; break;
	case PRIMITIVE_QUADS:
		drawShape = GL_QUADS; break;
	default:
		drawShape = GL_POINTS; break;
	}

	glDrawArrays(drawShape, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawQuads3DOld(const Vertex3 * vertexes, int numOfVertexes, const Texture* texture)
{
	//glEnable(GL_TEXTURE_2D);
	BindTexture(texture);
	glBegin(GL_QUADS);

	for (int vertexIndex = 0; vertexIndex < numOfVertexes; vertexIndex++)
	{
		const Vertex3& vertex = vertexes[vertexIndex];
		glColor4ub(vertex.color.red, vertex.color.green, vertex.color.blue, vertex.color.alpha);
		glTexCoord2f(vertex.texCoords.x, vertex.texCoords.y);
		glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
	}
	glEnd();
}

void Renderer::DrawLines3D(const Vertex3 * vertexes, int numOfVertexes)
{
	DrawVertexArray(vertexes, numOfVertexes, nullptr, PRIMITIVE_LINES);
	/*BindTexture(nullptr);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	constexpr int stride = sizeof(Vertex3);

	glVertexPointer(3, GL_FLOAT, stride, &vertexes[0].position);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, &vertexes[0].color);

	glDrawArrays(GL_LINES, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);*/
}

void Renderer::DrawLine3D(const Vector3 & start, const Vector3 & end, const Rgba & color)
{
	Vertex3 vertexes[] =
	{
		Vertex3(start,color),
		Vertex3(end,color)
	};
	int numOfVertexes = sizeof(vertexes) / sizeof(Vertex3);
	DrawLines3D(vertexes, numOfVertexes);
	/*BindTexture(nullptr);
	glBegin(GL_LINES);
	{
		glColor4ub(color.red, color.green, color.blue, color.alpha);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();*/
}

void Renderer::DrawAABB3(const AABB3& aabb, const Texture* texture, const AABB2& textureCoord, const Rgba& color)
{
	Vector3 mins = aabb.mins;
	Vector3 maxs = aabb.maxs;

	Vector3 delta = maxs - mins;
	
	Vector3 minsPlusX = mins + Vector3(delta.x, 0.f, 0.f);
	Vector3 minsPlusY = mins + Vector3(0.f, delta.y, 0.f);
	Vector3 minsPlusZ = mins + Vector3(0.f, 0.f, delta.z);
	Vector3 minsPlusXY = mins + Vector3(delta.x, delta.y, 0.f);
	Vector3 minsPlusXZ = mins + Vector3(delta.x, 0.f, delta.z);
	Vector3 minsPlusYZ = mins + Vector3(0.f, delta.y, delta.z);

	Vector2 texTopLeft(textureCoord.mins);
	Vector2 texBottomRight(textureCoord.maxs);
	Vector2 texBottomLeft(textureCoord.mins.x, textureCoord.maxs.y);
	Vector2 texTopRight(textureCoord.maxs.x, textureCoord.mins.y);

	Vertex3 vertexes[] =
	{
		//East (Plane YZ facing +X)
		Vertex3(minsPlusX	,color, texBottomLeft),
		Vertex3(minsPlusXY	,color, texBottomRight),
		Vertex3(maxs		,color, texTopRight),
		Vertex3(minsPlusXZ	,color, texTopLeft),

		//North (Plane XZ facing +Y)
		Vertex3(minsPlusXY	,color, texBottomLeft),
		Vertex3(minsPlusY	,color, texBottomRight),
		Vertex3(minsPlusYZ	,color, texTopRight),
		Vertex3(maxs		,color, texTopLeft),

		//West (Plane YZ facing -X)
		Vertex3(minsPlusY	,color, texBottomLeft),
		Vertex3(mins		,color, texBottomRight),
		Vertex3(minsPlusZ	,color, texTopRight),
		Vertex3(minsPlusYZ	,color, texTopLeft),

		//South (Plane XZ facing -Y)
		Vertex3(mins		,color, texBottomLeft),
		Vertex3(minsPlusX	,color, texBottomRight),
		Vertex3(minsPlusXZ	,color, texTopRight),
		Vertex3(minsPlusZ	,color, texTopLeft),

		//Top (Plane XY facing +Z)
		Vertex3(minsPlusZ	,color, texBottomLeft),
		Vertex3(minsPlusXZ	,color, texBottomRight),
		Vertex3(maxs		,color, texTopRight),
		Vertex3(minsPlusYZ	,color, texTopLeft),

		//Bottom (Plane XY facing -Z)
		Vertex3(minsPlusY	,color, texBottomLeft),
		Vertex3(minsPlusXY	,color, texBottomRight),
		Vertex3(minsPlusX	,color, texTopRight),
		Vertex3(mins		,color, texTopLeft),
	};
	int numOfVertexes = sizeof(vertexes) / sizeof(Vertex3);
	DrawQuads3D(vertexes, numOfVertexes, texture);
// 	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
// 	{
// 		DrawQuads(vertexes + faceIndex * 4, 4,texture);
// 	}


	/*
	
	Vertex3 vertexes[] =
	{
	//East (Plane YZ facing +X)
	Vertex3(minsPlusX	,Rgba(255,127,127,255), Vector2(0.f,1.f)),
	Vertex3(minsPlusXY	,Rgba(255,127,127,255), Vector2(1.f,1.f)),
	Vertex3(maxs		,Rgba(255,127,127,255), Vector2(1.f,0.f)),
	Vertex3(minsPlusXZ	,Rgba(255,127,127,255), Vector2(0.f,0.f)),

	//North (Plane XZ facing +Y)
	Vertex3(minsPlusXY	,Rgba(127,255,127,255), Vector2(0.f,1.f)),
	Vertex3(minsPlusY	,Rgba(127,255,127,255), Vector2(1.f,1.f)),
	Vertex3(minsPlusYZ	,Rgba(127,255,127,255), Vector2(1.f,0.f)),
	Vertex3(maxs		,Rgba(127,255,127,255), Vector2(0.f,0.f)),

	//West (Plane YZ facing -X)
	Vertex3(minsPlusY	,Rgba(0,127,127,255), Vector2(0.f,1.f)),
	Vertex3(mins		,Rgba(0,127,127,255), Vector2(1.f,1.f)),
	Vertex3(minsPlusZ	,Rgba(0,127,127,255), Vector2(1.f,0.f)),
	Vertex3(minsPlusYZ	,Rgba(0,127,127,255), Vector2(0.f,0.f)),

	//South (Plane XZ facing -Y)
	Vertex3(mins		,Rgba(127,0,127,255), Vector2(0.f,1.f)),
	Vertex3(minsPlusX	,Rgba(127,0,127,255), Vector2(1.f,1.f)),
	Vertex3(minsPlusXZ	,Rgba(127,0,127,255), Vector2(1.f,0.f)),
	Vertex3(minsPlusZ	,Rgba(127,0,127,255), Vector2(0.f,0.f)),

	//Top (Plane XY facing +Z)
	Vertex3(minsPlusZ	,Rgba(127,127,255,255), Vector2(0.f,1.f)),
	Vertex3(minsPlusXZ	,Rgba(127,127,255,255), Vector2(1.f,1.f)),
	Vertex3(maxs		,Rgba(127,127,255,255), Vector2(1.f,0.f)),
	Vertex3(minsPlusYZ	,Rgba(127,127,255,255), Vector2(0.f,0.f)),

	//Bottom (Plane XY facing -Z)
	Vertex3(minsPlusY	,Rgba(127,127,0,255), Vector2(0.f,1.f)),
	Vertex3(minsPlusXY	,Rgba(127,127,0,255), Vector2(1.f,1.f)),
	Vertex3(minsPlusX	,Rgba(127,127,0,255), Vector2(1.f,0.f)),
	Vertex3(mins		,Rgba(127,127,0,255), Vector2(0.f,0.f)),
	};
	
	*/
}

void Renderer::DrawTexturedAABB3(const AABB3 & aabb, const Texture * texture, const Vector2 * texCoords, const Rgba & color)
{
	Vector3 mins = aabb.mins;
	Vector3 maxs = aabb.maxs;

	Vector3 delta = maxs - mins;

	Vector3 minsPlusX = mins + Vector3(delta.x, 0.f, 0.f);
	Vector3 minsPlusY = mins + Vector3(0.f, delta.y, 0.f);
	Vector3 minsPlusZ = mins + Vector3(0.f, 0.f, delta.z);
	Vector3 minsPlusXY = mins + Vector3(delta.x, delta.y, 0.f);
	Vector3 minsPlusXZ = mins + Vector3(delta.x, 0.f, delta.z);
	Vector3 minsPlusYZ = mins + Vector3(0.f, delta.y, delta.z);

	Vertex3 vertexes[] =
	{
		//East (Plane YZ facing +X)
		Vertex3(minsPlusX	,color, texCoords[0]),
		Vertex3(minsPlusXY	,color, texCoords[1]),
		Vertex3(maxs		,color, texCoords[2]),
		Vertex3(minsPlusXZ	,color, texCoords[3]),

		//North (Plane XZ facing +Y)
		Vertex3(minsPlusXY	,color, texCoords[4]),
		Vertex3(minsPlusY	,color, texCoords[5]),
		Vertex3(minsPlusYZ	,color, texCoords[6]),
		Vertex3(maxs		,color, texCoords[7]),

		//West (Plane YZ facing -X)
		Vertex3(minsPlusY	,color, texCoords[8]),
		Vertex3(mins		,color, texCoords[9]),
		Vertex3(minsPlusZ	,color, texCoords[10]),
		Vertex3(minsPlusYZ	,color, texCoords[11]),

		//South (Plane XZ facing -Y)
		Vertex3(mins		,color, texCoords[12]),
		Vertex3(minsPlusX	,color, texCoords[13]),
		Vertex3(minsPlusXZ	,color, texCoords[14]),
		Vertex3(minsPlusZ	,color, texCoords[15]),

		//Top (Plane XY facing +Z)
		Vertex3(minsPlusZ	,color, texCoords[16]),
		Vertex3(minsPlusXZ	,color, texCoords[17]),
		Vertex3(maxs		,color, texCoords[18]),
		Vertex3(minsPlusYZ	,color, texCoords[19]),

		//Bottom (Plane XY facing -Z)
		Vertex3(minsPlusY	,color, texCoords[20]),
		Vertex3(minsPlusXY	,color, texCoords[21]),
		Vertex3(minsPlusX	,color, texCoords[22]),
		Vertex3(mins		,color, texCoords[23]),
	};

	int numOfVertexes = sizeof(vertexes) / sizeof(Vertex3);

	DrawQuads3D(vertexes, numOfVertexes, texture);

// 	for (int faceIndex = 0; faceIndex < 6; faceIndex++)
// 	{
// 		DrawQuads(vertexes + faceIndex * 4, 4,texture);
// 	}
}

void Renderer::DrawWiredCylinder3D(const Vector3 & centerPos, float height, float radius, int sides, const Rgba & color)
{
	std::vector<Vertex3> vertexes;
	float deltaDegrees = 360.f / static_cast<float>(sides);
	Vector3 top(centerPos + Vector3(0.f, 0.f, height*0.5f));
	Vector3 bottom(centerPos - Vector3(0.f, 0.f, height*0.5f));
	//bool topToDown = true;
	for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
	{
		Vector3 deltaArcPos(radius * CosDegrees(degrees), radius * SinDegrees(degrees), 0.f);
		Vector3 arcOnTop(top + deltaArcPos);
		Vector3 arcOnBottom(bottom + deltaArcPos);

		vertexes.push_back(Vertex3(top, color));
		vertexes.push_back(Vertex3(arcOnTop, color));
		vertexes.push_back(Vertex3(arcOnTop, color));
		vertexes.push_back(Vertex3(arcOnBottom, color));
		vertexes.push_back(Vertex3(arcOnBottom, color));
		vertexes.push_back(Vertex3(bottom, color));

		/*if (topToDown)
		{
			topToDown = false;
			vertexes.push_back(Vertex3(arcOnTop, color));
			vertexes.push_back(Vertex3(arcOnBottom, color));
			vertexes.push_back(Vertex3(bottom, color));
		}
		else 
		{
			topToDown = true;
			vertexes.push_back(Vertex3(arcOnBottom, color));
			vertexes.push_back(Vertex3(arcOnTop, color));
			vertexes.push_back(Vertex3(top, color));
		}*/
	}
	DrawLines3D(vertexes.data(), vertexes.size());
}


void Renderer::DrawSphere3D(const Vector3 & centerPos, float radius, int sides, const Rgba & color)
{
	std::vector<Vertex3> vertexes;
	float deltaDegrees = 360.f / static_cast<float>(sides);
	bool topToBottom = true;
	for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
	{
		if(topToBottom)
			for (float zDegrees = 0.f; zDegrees <= 180.f; zDegrees += deltaDegrees)
			{
				Vector3 pos(radius * CosDegrees(degrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
				vertexes.push_back(Vertex3(centerPos + pos, color));
			}
		else
			for (float zDegrees = 180.f; zDegrees >= 0.f; zDegrees -= deltaDegrees)
			{
				Vector3 pos(radius * CosDegrees(degrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
				vertexes.push_back(Vertex3(centerPos + pos, color));
			}
		topToBottom = !topToBottom;
	}
	DrawVertexArray(vertexes.data(), vertexes.size(), nullptr, PRIMITIVE_LINE_LOOP);
	vertexes.clear();
	for (float zDegrees = 0.f; zDegrees <= 180.f; zDegrees += deltaDegrees)
	{
		for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
		{
			Vector3 pos1(radius * CosDegrees(degrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
			Vector3 pos2(radius * CosDegrees(degrees + deltaDegrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees + deltaDegrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
			vertexes.push_back(Vertex3(centerPos + pos1, color));
			vertexes.push_back(Vertex3(centerPos + pos2, color));
		}
	}
	DrawVertexArray(vertexes.data(), vertexes.size(), nullptr, PRIMITIVE_LINES);
}

std::vector<Vertex3> Renderer::GetSphere3D(const Vector3 & centerPos, float radius, int sides, const Rgba & color)
{
	std::vector<Vertex3> vertexes;
	float deltaDegrees = 360.f / static_cast<float>(sides);
	for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
	{
		for (float zDegrees = 0.f; zDegrees < 180.f; zDegrees += deltaDegrees)
		{
			Vector3 pos(radius * CosDegrees(degrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
			float nextZDegrees = zDegrees + deltaDegrees;
			Vector3 nextPos(radius * CosDegrees(degrees) * SinDegrees(nextZDegrees), radius * SinDegrees(degrees) * SinDegrees(nextZDegrees), radius * CosDegrees(nextZDegrees));
			vertexes.push_back(Vertex3(centerPos + pos, color));
			vertexes.push_back(Vertex3(centerPos + nextPos, color));
		}
	}
	for (float zDegrees = 0.f; zDegrees <= 180.f; zDegrees += deltaDegrees)
	{
		for (float degrees = 0.f; degrees < 360.f; degrees += deltaDegrees)
		{
			Vector3 pos1(radius * CosDegrees(degrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
			Vector3 pos2(radius * CosDegrees(degrees + deltaDegrees) * SinDegrees(zDegrees), radius * SinDegrees(degrees + deltaDegrees) * SinDegrees(zDegrees), radius * CosDegrees(zDegrees));
			vertexes.push_back(Vertex3(centerPos + pos1, color));
			vertexes.push_back(Vertex3(centerPos + pos2, color));
		}
	}

	return vertexes;
}

void Renderer::GenerateVBOBuffer(unsigned int numOfBuffer, unsigned int* vboIds)
{
	glGenBuffers(numOfBuffer, vboIds);
}

void Renderer::UpdateVBOBuffer(unsigned int vboId, const std::vector<Vertex3>& vertexes)
{
	unsigned int vertexBytes = sizeof(Vertex3) * vertexes.size();
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vertexBytes, vertexes.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::ClearVBOBuffer(unsigned int vboId)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::DrawQuads3D(unsigned int vboId, unsigned int numOfVertexes, const Texture * texture)
{
	BindTexture(texture);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, position));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, color));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, texCoords));

	glDrawArrays(GL_QUADS, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	BindTexture(nullptr);
}

void Renderer::DrawTriangles3D(unsigned int vboId, unsigned int numOfVertexes, const Texture * texture)
{
	BindTexture(texture);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, position));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, color));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3), (const GLvoid*)offsetof(Vertex3, texCoords));

	glDrawArrays(GL_TRIANGLE_MESH_SUN, 0, numOfVertexes);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	BindTexture(nullptr);
}


