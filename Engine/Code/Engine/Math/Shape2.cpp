#include "Engine/Math/Shape2.hpp"

Shape2::Shape2()
	: vertexes(nullptr)
	, numberOfVertexes(0)
{
}

Shape2::Shape2(const Shape2 & shape)
	: numberOfVertexes(shape.numberOfVertexes)
	, vertexes(nullptr)
{
	if (shape.vertexes)
	{
		vertexes = new Vertex2[numberOfVertexes];
		for (int i = 0; i < numberOfVertexes; i++)
		{
			vertexes[i] = Vertex2(shape.vertexes[i]);
		}
	}
}

Shape2::Shape2(const Vertex2 * a_vertexes, int a_numberOfVertexes)
	: numberOfVertexes(a_numberOfVertexes)
	, vertexes(nullptr)
{
	if (a_vertexes)
	{
		vertexes = new Vertex2[numberOfVertexes];
		for (int i = 0; i < numberOfVertexes; i++)
		{
			vertexes[i] = Vertex2(a_vertexes[i]);
		}
	}
}

Shape2::~Shape2()
{
	if (vertexes) 
	{
		delete vertexes;
		vertexes = nullptr;
	}
}

void Shape2::SetShape2(const Shape2 & shape)
{
	numberOfVertexes = shape.numberOfVertexes;
	if (shape.vertexes)
	{
		vertexes = new Vertex2[numberOfVertexes];
		for (int i = 0; i < numberOfVertexes; i++)
		{
			vertexes[i] = Vertex2(shape.vertexes[i]);
		}
	}
}

void Shape2::SetShape2(const Vertex2 * a_vertexes, int a_numberOfVertexes)
{
	SetShape2(Shape2(a_vertexes, a_numberOfVertexes));
}

const Shape2 & Shape2::operator=(const Shape2 & shape)
{
	SetShape2(shape);
	return *this;
}
