#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vertex2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Texture.hpp"
#include <vector>
#include "Engine/Math/Capsule2.hpp"
#include <string>
#include "BitmapFont.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vertex3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Matrix4.hpp"

extern const int PRIMITIVE_POINTS;
extern const int PRIMITIVE_LINES;
extern const int PRIMITIVE_LINE_LOOP;
extern const int PRIMITIVE_TRIANGLES;
extern const int PRIMITIVE_QUADS;

class Renderer
{
private:

	static const std::string DEFAULT_TEXTURE_NAME;

	std::vector< Texture* >		m_alreadyLoadedTextures;
	std::vector< BitmapFont* >	m_alreadyLoadedBitmapFonts;

public:
	Renderer();

	void Finish();

	void ClearScreen(float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);
	void ClearScreen(Rgba color);

	void PushMatrix();
	void PopMatrix();

	void ClearDepth();
	void EnableDepthTesting(bool isNowDepthTesting = true);

	Texture* CreateOrGetTexture(const std::string& imageFilePath);
	Texture* CreateTextureFromFile(const std::string & imageFilePath);
	Texture* GetTexture(const std::string & imageFilePath);
	unsigned int CreateOpenGLTexture(unsigned char * imageTexelBytes, int width, int height, int bytesPerTexel);

	BitmapFont* CreateOrGetBitFont(const std::string& fontName);
	BitmapFont* CreateBitFontFromFile(const std::string & fontName);
	BitmapFont* GetBitFont(const std::string & fontName);

	void CreateDefaultTexture();
	void BindTexture(const Texture* texture);

	void SetColor(Rgba color);
	void SetAdditiveBlending();
	void SetSubtractiveBlending();
	void SetAlphaBlending();
	void EnableBackFaceCulling(bool isNowBackFaceCulling = true);

	//2D

	void Translate2D(const Vector2& translation);
	void Rotate2D(float degrees);
	void Scale2D(const Vector2& scale);

	void DrawPoint2(const Vector2& pos, const Rgba& color = Rgba::WHITE);
	void DrawLine(const Vector2& start, const Vector2& end, const Rgba& startColor = Rgba::WHITE, const Rgba& endColor = Rgba::WHITE, float thickness = -1.f);
	void DrawLine(const Ray2& line, const Rgba& color = Rgba::WHITE, float thickness = -1.f);
	void DrawLineStrip(const Vertex2* vertexArray, int numOfVertexes);
	void DrawRagularPolygonOutline(float x, float y, float sides, float radius, float offsetDegree = 0.f, Rgba color = Rgba::WHITE);
	void DrawCircle(float radius, float sides = 64, Rgba color = Rgba::WHITE);
	void DrawCircle(float x, float y, float radius, float sides = 64, Rgba color = Rgba::WHITE);
	void DrawShape(const Vertex2* vertexes, int numberOfVertexes);
	void DrawShape(const Vector2& position, const Vector2* points, int numberOfPoints, float offsetDegree = 0.f, Rgba color = Rgba::WHITE);
	void DrawShape(const Vector2& position, const Vertex2* points, int numberOfVertexes, float offsetDegree = 0.f);
	void DrawDisc2(float radius, const Rgba& color = Rgba::WHITE, float sides = 24.f);
	void DrawDisc2(const Vector2& position, float radius, const Rgba& color = Rgba::WHITE, float sides = 24.f);
	void DrawAABB2(const AABB2& bounds, const Rgba& color = Rgba::WHITE);
	void DrawCapsule2(const Capsule2& capsule, const Rgba& color);
	//void DrawTexturedAABB2(const AABB2& bounds, const Texture& texture);
	void DrawTexturedAABB2(const AABB2& bounds, const Texture& texture, const AABB2& textureCoods = AABB2::ZERO_TO_ONE, const Rgba& color = Rgba::WHITE);
	void DrawTexturedAABB2(const AABB2& bounds, const Texture* texture, const AABB2& textureCoods = AABB2::ZERO_TO_ONE, const Rgba& color = Rgba::WHITE);
	//void DrawText(const std::string& text, const Vector2& bottomLeftStartPosition, float fontHeight, float fontAspect, const Rgba& color);
	void DrawText2(const Vector2& startTopLeft, const std::string& text, float drawWidth, const BitmapFont& font, const Rgba& color = Rgba::WHITE, float fontAspect = BitmapFont::DEFAULT_BITMAP_FONT_ASPECT, const std::string& align = "LEFT");

	void SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight);
	void SetOrthoProjection(float bottomLeftX, float bottomLeftY,float topRightX, float topRightY);
	void SetOrthoProjection(const AABB2& bound);
	void SetOrthoCentered(const Vector2& center, const Vector2& size);
	void SetOrthoCentered(float centerX, float centerY, float sizeX, float sizeY);
	void RotateAroundPosition2D(const Vector2& position, float Degrees);

	//3D

	void SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float nearDistance, float farDistance);
	void Translate3D(const Vector3& translation);
	void Translate3D(float x, float y, float z);
	void Rotate3D(float degrees, float rotateAxisX, float rotateAxisY, float rotateAxisZ);
	void Rotate3D(float degrees, Vector3 basis);
	void Scale3D(float x, float y, float z);
	void Scale3D(const Vector3& scales);
	void Scale3D(float uniformScale);
	void GetCurrentMatrix(float* matrix);
	void LoadMatrix(const Matrix4& matrix);
	void MultplyMatrix(const Matrix4& matrix);
	void LookAt(const Vector3& eyePosition, const Vector3& lookPosition, const Vector3& upDirection);

	void DrawAxes(float lengthX, float lengthY, float lengthZ);
	
	void DrawQuads3DOld(const Vertex3* vertexes, int numOfVertexes, const Texture* texture = nullptr);
	void DrawLines3D(const Vertex3* vertexes, int numOfVertexes);
	void DrawLine3D(const Vector3& start, const Vector3& end, const Rgba& color = Rgba::WHITE);
	void DrawAABB3(const AABB3& aabb, const Texture* texture, const AABB2& textureCoord = AABB2::ZERO_TO_ONE, const Rgba& color = Rgba::WHITE);
	void DrawTexturedAABB3(const AABB3& aabb, const Texture* texture, const Vector2* texCoords,const Rgba& color = Rgba::WHITE);
	void DrawWiredCylinder3D(const Vector3& centerPos, float height, float radius, int sides = 64, const Rgba& color = Rgba::WHITE);
	void DrawSphere3D(const Vector3& centerPos, float radius, int sides = 64, const Rgba& color = Rgba::WHITE);

	//GetVertices

	std::vector<Vertex3> GetSphere3D(const Vector3& centerPos, float radius, int sides = 64, const Rgba& color = Rgba::WHITE);

	//modren drawing

	void DrawPoints3D(const Vertex3* vertexes, int numOfVertexes);
	void DrawQuads3D(const Vertex3* vertexes, int numOfVertexes, const Texture* texture = nullptr);
	void DrawVertexArray(const Vertex3* vertexes, int numOfVertexes, const Texture* texture = nullptr, int primitiveType = PRIMITIVE_QUADS);
	void GenerateVBOBuffer(unsigned int numOfBuffer, unsigned int* vboIds);
	void UpdateVBOBuffer(unsigned int vboId, const std::vector<Vertex3>& vertexes);
	void ClearVBOBuffer(unsigned int vboId);
	void DrawQuads3D(unsigned int vboId, unsigned int numOfVertexes, const Texture* texture = nullptr);
	void DrawTriangles3D(unsigned int vboId, unsigned int numOfVertexes, const Texture* texture = nullptr);

};

