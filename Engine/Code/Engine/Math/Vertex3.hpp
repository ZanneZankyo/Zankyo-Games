#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/Vector4.hpp"

class Vertex3
{
public:
	Vector3 position;
	Rgba color;
	Vector2 texCoords;
	Vector3 normal;
	Vector3 tangent;
	UIntVector4 boneIndices;
	Vector4 boneWeights;
public:
	Vertex3();
	Vertex3(const Vertex3& copy);
	explicit Vertex3(
		const Vector3& pos, 
		const Rgba& color = Rgba::WHITE, 
		const Vector2& texCoods = Vector2::ZERO, 
		const Vector3& normal = Vector3(0.f,0.f,0.f), 
		const Vector3& tangent = Vector3(0.f, 0.f, 0.f), 
		const UIntVector4& boneIndices = UIntVector4::ZERO, 
		const Vector4& boneWeights = Vector4(1.f,0.f,0.f,0.f));
	explicit Vertex3(const Vector3& pos, Vector2 texCoods, const Vector3& normal = Vector3(0.f, 0.f, 0.f), const Vector3& tangent = Vector3(0.f, 0.f, 0.f));
	bool operator== (const Vertex3& vertex);
};

inline Vertex3::Vertex3()
	: position()
	, color(Rgba::WHITE)
	, texCoords(Vector2::ZERO)
	, normal(0.f, 0.f, 1.f)
	, tangent(1.f,0.f,0.f)
	, boneIndices(0,0,0,0)
	, boneWeights(1.f,0.f,0.f,0.f)
{}

inline Vertex3::Vertex3(const Vertex3 & copy)
	: position(copy.position)
	, color(copy.color)
	, texCoords(copy.texCoords)
	, normal(copy.normal)
	, tangent(copy.tangent)
	, boneIndices(copy.boneIndices)
	, boneWeights(copy.boneWeights)
{}

inline Vertex3::Vertex3(
	const Vector3&		p_pos,
	const Rgba&			p_color			/*= Rgba::WHITE*/,
	const Vector2&		p_texCoods		/*= Vector2::ZERO*/,
	const Vector3&		p_normal		/*= Vector3(0.f, 0.f, 0.f)*/,
	const Vector3&		p_tangent		/*= Vector3(0.f, 0.f, 0.f)*/,
	const UIntVector4&	p_boneIndices	/*= UIntVector4::ZERO*/,
	const Vector4&		p_boneWeights	/*= Vector4::ZERO*/)
	: position(p_pos)
	, color(p_color)
	, texCoords(p_texCoods)
	, normal(p_normal)
	, tangent(p_tangent)
	, boneIndices(p_boneIndices)
	, boneWeights(p_boneWeights)
{}

inline Vertex3::Vertex3(const Vector3 & pos, const Vector2 texC, const Vector3& n, const Vector3& t)
	: position(pos)
	, color(Rgba::WHITE)
	, texCoords(texC)
	, normal(n)
	, tangent(t)
	, boneIndices(0, 0, 0, 0)
	, boneWeights(1.f, 0.f, 0.f, 0.f)
{}

inline bool Vertex3::operator==(const Vertex3 & vertex)
{
	return 
		position == vertex.position && 
		color == vertex.color && 
		texCoords == vertex.texCoords && 
		normal == vertex.normal && 
		tangent == vertex.tangent &&
		boneIndices == vertex.boneIndices &&
		boneWeights == vertex.boneWeights;
}
