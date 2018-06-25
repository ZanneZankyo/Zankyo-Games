#pragma once
#include "Engine/Math/Vertex2.hpp"

class Shape2
{
public:
	Vertex2* vertexes;
	int numberOfVertexes;
public:
	Shape2();
	Shape2(const Shape2& shape);
	Shape2(const Vertex2* a_vertexes, int a_numberOfVertexes);
	~Shape2();

	void SetShape2(const Shape2& shape);
	void SetShape2(const Vertex2* a_vertexes, int a_numberOfVertexes);

	const Shape2& operator=(const Shape2& shape);
};