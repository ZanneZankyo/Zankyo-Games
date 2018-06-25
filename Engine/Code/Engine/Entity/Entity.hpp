#pragma once
#include "Engine/Math/Vector2.hpp"

class Entity
{
protected:
	Vector2		m_position;
	Vector2		m_velocity;

	float		m_orientationDegrees;
	float		m_angularVelocity;

	float		m_physicsRadius;
	float		m_cosmeticRadius;

protected:

	void Update(float deltaSeconds);
	void Render() const;

public:
	
	Entity();
	Entity(const Entity& copy);
	Entity(const Vector2& a_position, const Vector2& a_velocity = Vector2(0.f,0.f)
		, float a_orientationDegrees = 0.f, float a_angularVelocity = 0.f
		, float a_physicsRadius = 0.f, float a_cosmeticRadius = 0.f);
	~Entity();

	Vector2 GetPosition() const;
	Vector2 GetVelocity() const;
	float GetOrientationDegrees() const;
	float GetAngularVelocity() const;
	float GetPhysicsRadius() const;
	float GetCosmeticRadius() const;
	Entity GetEntity() const;

	void SetEntity(const Entity& copy);

	const Entity& operator = (const Entity& copy);
	bool operator == (const Entity& entityToEqual) const;
	bool operator != (const Entity& entityNotToEqual) const;

	friend bool IsOverlap(const Entity& a, const Entity& b);
};