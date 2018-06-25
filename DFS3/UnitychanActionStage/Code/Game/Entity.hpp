#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\PhysicsDynamicActor.hpp"

class Controller;

class Entity
{
public:
	Vector3 m_position;
	Vector3 m_velocity;
	Vector3 m_rotation;
	Controller* m_controller = nullptr;

	PhysicsDynamicActor* m_pxActor = nullptr;
public:
	virtual void Update();
	virtual void Render() const;

	virtual void SetPosition(const Vector3& position);
};