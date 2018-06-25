#ifdef TOOLS_BUILD

#include "PhysicsSpringJoint.hpp"
#include "Physics.hpp"
#include "PhysicsDynamicActor.hpp"
#include "..\Core\Time.hpp"
#include "MathUtils.hpp"

PhysicsSpringJoint::PhysicsSpringJoint(PhysicsDynamicActor* physicsActor, SkeletonJoint* joint, SkeletonInstance* skeletonIns)
	: m_joint(joint)
	, m_physicsActor(physicsActor)
	, m_parent(nullptr)
	, m_pxJoint(nullptr)
	, m_skeletonIns(skeletonIns)
{
}

PhysicsSpringJoint* PhysicsSpringJoint::PairJoint(PhysicsDynamicActor* childActor, SkeletonJoint* childJoint)
{
	PhysicsSpringJoint* childPhysicsJoint = new PhysicsSpringJoint(childActor, childJoint, m_skeletonIns);
	
	Matrix4 localFrame = m_skeletonIns->m_currentPose.GetTransform(m_skeletonIns->m_skeleton->GetJointIndex(childJoint));
	Matrix4 localRotation;
	Vector3 localPosition;
	localFrame.Decompose(&localPosition, &localRotation, nullptr);

	float jointDistance = (m_joint->m_transform.GetPosition() - childJoint->m_transform.GetPosition()).CalcLength();

	m_pxJoint = PxSphericalJointCreate(*PhysicsSystem::GetPxPhysics(),
		m_physicsActor->m_pxRigidActor, PxTransform(PxIdentity),//ToPxTransform(Matrix4::CreateTranslation(Vector3(localPosition.CalcLength(),0.f,0.f))),//ToPxTransform(Matrix4::CreateTranslation(Vector3(jointDistance,0.f,0.f))),//
		childActor->m_pxRigidActor, PxTransform(PxIdentity)/*ToPxTransform(localRotation)*/);
	//PxJointLimitCone limitCone(PxPi / 2, PxPi / 6, 0.01f);
	PxJointLimitCone limitCone(0.01f, 0.01f);
	limitCone.stiffness = 10.f;
	m_pxJoint->setLimitCone(limitCone);
	m_pxJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	m_children.push_back(childPhysicsJoint);
	childPhysicsJoint->m_parent = this;
	return childPhysicsJoint;
}

void PhysicsSpringJoint::Update(Pose& currentPose)
{
	int jointIndex = m_skeletonIns->GetJointIndex(m_joint);
	if (!m_parent)
	{
		m_physicsActor->SetTransform(m_skeletonIns->GetJointGlobalTransform(jointIndex));
		//m_physicsActor->SetTransform(currentPose.GetTransform(jointIndex));
	}
	else
	{
		m_skeletonIns->SetGlobalTransfrom(jointIndex, m_physicsActor->GetTransform());
		//if (m_parent)
		//	currentPose.GetTransform(jointIndex) = m_physicsActor->GetTransform() * m_parent->m_physicsActor->GetTransform().GetInverse();
	}
	for (auto& child : m_children)
		child->Update(currentPose);
}

#endif
