#include "Engine/Entity/Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

void Entity::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += m_angularVelocity * deltaSeconds;

}

void Entity::Render() const
{
	if (g_input->IsKeyDown('E'))
	{
		g_renderer->DrawCircle(m_position.x, m_position.y, m_physicsRadius);
		g_renderer->DrawCircle(m_position.x, m_position.y, m_cosmeticRadius);
	}
}

Entity::Entity()
	: m_position()
	, m_velocity()
	, m_orientationDegrees(0.f)
	, m_angularVelocity(0.f)
	, m_physicsRadius(0.f)
	, m_cosmeticRadius(0.f)
{
}

Entity::Entity(const Entity & copy)
	: m_position(copy.m_position)
	, m_velocity(copy.m_velocity)
	, m_orientationDegrees(copy.m_orientationDegrees)
	, m_angularVelocity(copy.m_angularVelocity)
	, m_physicsRadius(copy.m_physicsRadius)
	, m_cosmeticRadius(copy.m_cosmeticRadius)
{
}

Entity::Entity(const Vector2 & a_position, const Vector2 & a_velocity, float a_orientationDegrees, float a_angularVelocity, float a_physicsRadius, float a_cosmeticRadius)
	: m_position(a_position)
	, m_velocity(a_velocity)
	, m_orientationDegrees(a_orientationDegrees)
	, m_angularVelocity(a_angularVelocity)
	, m_physicsRadius(a_physicsRadius)
	, m_cosmeticRadius(a_cosmeticRadius)
{
}

Entity::~Entity()
{
}

Vector2 Entity::GetPosition() const
{
	return m_position;
}

Vector2 Entity::GetVelocity() const
{
	return m_velocity;
}

float Entity::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}

float Entity::GetAngularVelocity() const
{
	return m_angularVelocity;
}

float Entity::GetPhysicsRadius() const
{
	return m_physicsRadius;
}

float Entity::GetCosmeticRadius() const
{
	return m_cosmeticRadius;
}

Entity Entity::GetEntity() const
{
	return *this;
}

void Entity::SetEntity(const Entity & copy)
{
	m_position = copy.m_position;
	m_velocity = copy.m_velocity;
	m_orientationDegrees = copy.m_orientationDegrees;
	m_angularVelocity = copy.m_angularVelocity;
	m_physicsRadius = copy.m_physicsRadius;
	m_cosmeticRadius = copy.m_cosmeticRadius;
}

const Entity & Entity::operator=(const Entity & copy)
{
	SetEntity(copy);
	return *this;
}

bool Entity::operator==(const Entity & entityToEqual) const
{
	return m_position == entityToEqual.m_position
		&& m_velocity == entityToEqual.m_velocity
		&& m_orientationDegrees == entityToEqual.m_orientationDegrees
		&& m_angularVelocity == entityToEqual.m_angularVelocity
		&& m_physicsRadius == entityToEqual.m_physicsRadius
		&& m_cosmeticRadius == entityToEqual.m_cosmeticRadius;
}

bool Entity::operator!=(const Entity & entityNotToEqual) const
{
	return !(*this == entityNotToEqual);
}

bool IsOverlap(const Entity & a, const Entity & b)
{
	return DoDiscsOverlap(a.m_position, a.m_physicsRadius, b.m_position, b.m_physicsRadius);
}
