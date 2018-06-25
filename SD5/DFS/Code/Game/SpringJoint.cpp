#include "SpringJoint.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\Skeleton.hpp"

SpringJoint::SpringJoint(SkeletonJoint* joint, SkeletonJoint* child, SkeletonInstance* instance)
	: m_owner(joint)
	, m_child(child)
	, m_instance(instance)
{
	m_childJointIndex = instance->GetJointIndex(m_child);
	m_parentJointIndex = instance->GetJointIndex(m_owner->m_parent);

	m_springLength = (joint->m_transform.GetPosition() - m_child->m_transform.GetPosition()).CalcLength();
	m_currTipPos = child->m_transform.GetPosition();
	m_prevTipPos = m_currTipPos;
	/*if (joint->m_parent)
		m_localTransform = joint->m_transform * joint->m_parent->m_inverseTransform;
	else
		m_localTransform = joint->m_transform;*/
	//m_trs = joint->m_transform;
	int jointIndex = instance->m_skeleton->GetJointIndex(child);
	
	m_jointIndex = instance->GetJointIndex(m_owner);

	m_localTransform = instance->m_currentPose.m_transforms[m_jointIndex];
	m_localRotation = instance->m_currentPose.m_transforms[m_jointIndex].GetRotation();
	m_currentRotation = m_localRotation;
}

void SpringJoint::Update()
{
	Update(Time::deltaSeconds);
}

void SpringJoint::Update(float deltaSeconds)
{
	m_org = m_instance->m_currentPose.GetTransform(m_jointIndex);

	m_instance->m_currentPose.GetTransform(m_jointIndex).SetRotation(m_localRotation);
	m_localTransform = m_instance->m_currentPose.GetTransform(m_jointIndex);

	Matrix4 parentMat = m_instance->GetJointGlobalTransform(m_parentJointIndex);
	Matrix4 globalMat = m_instance->GetJointGlobalTransform(m_jointIndex);
	//Matrix4 childMat = m_instance->GetJointGlobalTransform(m_childJointIndex);
	Matrix4 globalInv = globalMat.GetInverse();
	Matrix4 globalRotation;
	Vector3 globalPosition;
	Matrix4 parentRotation;
	globalMat.Decompose(&globalPosition, &globalRotation, nullptr);

	//回転をリセット
	m_localTransform.SetRotation(m_localRotation);
	Matrix4 rotation = m_localRotation;

	float sqrDt = deltaSeconds * deltaSeconds;

	//stiffness
	Vector3 force = globalRotation.TransformDirection((m_boneAxis * m_stiffnessForce) / sqrDt);

	//drag
	force += (m_prevTipPos - m_currTipPos) * m_dragForce / sqrDt;
	force += m_springForce / sqrDt;

	//前フレームと値が同じにならないように
	Vector3 temp = m_currTipPos;

	//verlet
	m_currTipPos = (m_currTipPos - m_prevTipPos) + m_currTipPos + (force * sqrDt);

	//長さを元に戻す set to original length
	Vector3 toChildDirection = m_currTipPos - globalPosition;
	float distance = toChildDirection.CalcLength();
	float fixDis = MIN(distance, m_springLength);
	//Vector3 toChildDirection = childMat.GetPosition() - globalPosition;
	toChildDirection.Normalize();
	m_currTipPos = (toChildDirection * fixDis) + globalPosition;

	m_prevTipPos = temp;

	//回転を適用； apply rotation;
	Vector3 aimVector = globalRotation.TransformDirection(m_boneAxis);
	Vector3 direction = m_currTipPos - globalPosition;
	Matrix4 aimRotation = Matrix4::CreateFromToRotation(aimVector, direction);

	Matrix4 secondaryRotation = aimRotation * globalRotation;
	globalRotation.SetRotation(Interpolate(globalRotation, secondaryRotation, 0.7f));
	m_localTransform.SetRotation(parentMat.GetRotation().GetInverse() * globalRotation);
	m_instance->m_currentPose.GetTransform(m_jointIndex) = m_localTransform;
}
