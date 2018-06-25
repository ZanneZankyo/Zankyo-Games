#pragma once

#ifdef TOOLS_BUILD

#include "PhysicsTypes.hpp"
#include <PxPhysicsAPI.h>
using namespace physx;

class PhysicsActor
{
public:
	PhysicsActor(PxActor* pxActor);
private:
	PxActor* m_pxActor;
};

#endif