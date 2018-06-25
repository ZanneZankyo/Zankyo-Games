#include "PhysicsScene.hpp"
#include "Engine\Core\Time.hpp"
#include "PhysicsDynamicActor.hpp"
#include "PhysicsStaticActor.hpp"

#ifdef TOOLS_BUILD

PhysicsScene::PhysicsScene(PxScene* pxScene)
	: m_pxScene(pxScene)
{
	m_pxScene->userData = this;
}

PhysicsScene::~PhysicsScene()
{
	m_pxScene->release();
}

void PhysicsScene::Update()
{
	m_pxScene->fetchResults(true);
	m_pxScene->simulate(Time::deltaSeconds);
}

void PhysicsScene::AddActor(PhysicsDynamicActor* actor)
{
	m_pxScene->addActor(*actor->m_pxRigidActor);
}

void PhysicsScene::AddActor(PhysicsStaticActor* actor)
{
	m_pxScene->addActor(*actor->m_pxStaticActor);
}

#endif