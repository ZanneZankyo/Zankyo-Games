#pragma once
#include "Entity.hpp"
#include "Engine\Core\Event.hpp"

class HitBox : public Entity
{
public:
	float m_lifeSpan = 0.1f;
	Event<> m_onOverlap;
public:
	virtual void Update() override
	{
		Entity::Update();
	}


	virtual void Render() const override
	{}

};