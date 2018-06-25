#ifdef TOOLS_BUILD

#include "PhysicsDynamicActor.hpp"
#include "PhysicsShape.hpp"

#include <extensions\PxRigidBodyExt.h>

PhysicsDynamicActor::PhysicsDynamicActor(PxRigidDynamic* pxRigidActor)
	: m_pxRigidActor(pxRigidActor)
{
	m_pxRigidActor->userData = this;
}

void PhysicsDynamicActor::AddShape(PhysicsShape* shape)
{
	m_pxRigidActor->attachShape(*shape->m_pxShape);
	PxRigidBodyExt::updateMassAndInertia(*m_pxRigidActor, 1.f);
}

void PhysicsDynamicActor::Transform(const Matrix4& matrix)
{
	Matrix4 current = ToMatrix4(m_pxRigidActor->getGlobalPose());
	Matrix4 after = matrix * current;
	SetTransform(after);
}

void PhysicsDynamicActor::SetTransform(const Matrix4& matrix)
{
	m_pxRigidActor->setGlobalPose(ToPxTransform(matrix));
}

void PhysicsDynamicActor::SetMass(float mass)
{
	m_pxRigidActor->setMass(mass);
}

void PhysicsDynamicActor::SetLinearVelocity(const Vector3& linearVelocity, bool autoWake /*= true*/)
{
	m_pxRigidActor->setLinearVelocity(ToPxVec3(linearVelocity), autoWake);
}

Matrix4 PhysicsDynamicActor::GetTransform() const
{
	PxTransform transform = m_pxRigidActor->getGlobalPose();
	return ToMatrix4(transform);
}

void PhysicsDynamicActor::SetAngularLockX(bool isLocked)
{
	m_pxRigidActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, isLocked);
}

void PhysicsDynamicActor::SetAngularLockY(bool isLocked)
{
	m_pxRigidActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, isLocked);
}

void PhysicsDynamicActor::SetAngularLockZ(bool isLocked)
{
	m_pxRigidActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, isLocked);
}

#endif