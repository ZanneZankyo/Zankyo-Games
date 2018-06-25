#pragma once

#ifdef TOOLS_BUILD

#include "PhysicsTypes.hpp"
#include <PxPhysicsAPI.h>
using namespace physx;

class PhysicsStaticActor
{
	friend class PhysicsScene;
public:
	PhysicsStaticActor(PxRigidStatic* pxStaticActor);
	void AddShape(PhysicsShape* shape);
	void Transform(const Matrix4& matrix) const;
	Matrix4 GetTransform() const;
public:
	void* m_userData = nullptr;
protected:
	PxRigidStatic* m_pxStaticActor;
};

#endif