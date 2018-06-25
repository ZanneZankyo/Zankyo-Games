#pragma once

#ifdef TOOLS_BUILD

#include "PhysicsTypes.hpp"
#include <PxPhysicsAPI.h>
using namespace physx;

class PhysicsDynamicActor
{
	friend class PhysicsSystem;
	friend class PhysicsScene;
	friend class PhysicsSpringJoint;
public:
	PhysicsDynamicActor(PxRigidDynamic* pxRigidActor);
	void AddShape(PhysicsShape* shape);
	void Transform(const Matrix4& matrix);
	void SetTransform(const Matrix4& matrix);
	void SetMass(float mass);
	void SetLinearVelocity(const Vector3& linearVelocity, bool autoWake = true);
	Matrix4 GetTransform() const;
	void SetRotationFreedom();
	void SetAngularLockX(bool isLocked);
	void SetAngularLockY(bool isLocked);
	void SetAngularLockZ(bool isLocked);
public:
	void* m_userData = nullptr;
protected:
	PxRigidDynamic* m_pxRigidActor = nullptr;
};

#endif