#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/RGBA.hpp"

class Vertex2
{
public:
	Vector2 position;
	Rgba color;
	Vector2 texCoords;
public:
	Vertex2();
	Vertex2(const Vertex2& copy);
	explicit Vertex2(const Vector2& pos, const Rgba& color = Rgba::WHITE);
};