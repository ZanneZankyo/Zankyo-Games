#pragma once

#ifdef TOOLS_BUILD

#include "Matrix4.hpp"
#include "PhysicsTypes.hpp"
#include <PxPhysicsAPI.h>

using namespace physx;

class PhysicsScene
{
	friend class PhysicsSystem;
protected:
	PxScene* m_pxScene;
public:
	PhysicsScene(PxScene* pxScene);
	~PhysicsScene();
	void Update();
	void AddActor(PhysicsDynamicActor* actor);
	void AddActor(PhysicsStaticActor* actor);
};

#endif