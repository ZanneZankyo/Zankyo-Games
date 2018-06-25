#include "PhysicsShape.hpp"

#ifdef TOOLS_BUILD

PhysicsShape::PhysicsShape(PxShape* pxShape)
	: m_pxShape(pxShape)
{
	m_pxShape->userData = this;
}

void PhysicsShape::SetTransform(const Matrix4& transform)
{
	m_pxShape->setLocalPose(ToPxTransform(transform));
}

#endif