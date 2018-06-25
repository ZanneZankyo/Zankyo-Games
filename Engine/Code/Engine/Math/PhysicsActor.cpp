#include "PhysicsActor.hpp"

#ifdef TOOLS_BUILD

PhysicsActor::PhysicsActor(PxActor* pxActor)
	: m_pxActor(pxActor)
{
	m_pxActor->userData = this;
}

#endif