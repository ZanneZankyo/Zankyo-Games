#ifdef TOOLS_BUILD

#include "PhysicsStaticActor.hpp"

#include <extensions\PxRigidBodyExt.h>
#include "PhysicsShape.hpp"

PhysicsStaticActor::PhysicsStaticActor(PxRigidStatic* pxStaticActor)
	: m_pxStaticActor(pxStaticActor)
{
	m_pxStaticActor->userData = this;
}

void PhysicsStaticActor::AddShape(PhysicsShape* shape)
{
	m_pxStaticActor->attachShape(*shape->m_pxShape);
	//PxRigidBodyExt::updateMassAndInertia(*m_pxRigidActor, 1.f);
}

void PhysicsStaticActor::Transform(const Matrix4& matrix) const
{
	Matrix4 current = ToMatrix4(m_pxStaticActor->getGlobalPose());
	Matrix4 after = matrix * current;
	m_pxStaticActor->setGlobalPose(ToPxTransform(after));
}

Matrix4 PhysicsStaticActor::GetTransform() const
{
	PxTransform transform = m_pxStaticActor->getGlobalPose();
	return ToMatrix4(transform);
}

#endif