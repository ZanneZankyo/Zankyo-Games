#include "Engine/Math/Vertex2.hpp"


Vertex2::Vertex2()
	: position()
	, color()
{}

Vertex2::Vertex2(const Vertex2 & copy)
	: position(copy.position)
	, color(copy.color)
{
}

Vertex2::Vertex2(const Vector2 & pos, const Rgba & c)
	: position(pos)
	, color(c)
{}
