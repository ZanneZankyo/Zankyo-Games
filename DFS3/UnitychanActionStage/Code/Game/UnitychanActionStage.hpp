#pragma once
#include <vector>
#include "Entity.hpp"
#include "Controller.hpp"
#include "Engine\Renderer\Light.hpp"
#include "Unitychan.hpp"
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\Math\PhysicsScene.hpp"
#include "Engine\Math\PhysicsStaticActor.hpp"

class UnitychanActionStage
{
public:
	Controller m_controller;
	Unitychan* m_player = nullptr;
	std::vector<Entity*> m_entities;
	Light m_light;

	VertexBuffer* m_gridVbo = nullptr;

	PhysicsScene* m_pxScene = nullptr;

	PhysicsStaticActor* m_floor = nullptr;
public:
	void Start();
	void Update();
	void Render() const;
	void AddEntity(Entity* entity);
	void BuildVbo();

	static UnitychanActionStage* GetInstance();
private:
	void SetupFloor();
};