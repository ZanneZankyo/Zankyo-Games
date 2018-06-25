#include "Animation.hpp"
#include "Skeleton.hpp"
#include "Motion.hpp"
#include "Engine\Math\MatrixStack.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "..\Tools\fbx.hpp"

std::map<std::string, Animation*> Animation::s_loadedAnimations;

AnimCurve& Animation::GetCurveByName(const std::string& name)
{
	for (auto& curve : m_curves)
		if (curve.m_name == name)
			return curve;
	ASSERT_OR_DIE(false, "Animation::GetCurveByName():\nCurve name %s not found!\n");
	//return AnimCurve();
}

void Animation::EvaluateSkinning(Pose& skinningPose, Skeleton * skeleton, float time) const
{

	time = fmod(time, m_duration);

	size_t jointCount = skeleton->GetJointCount();
	skinningPose.m_transforms.resize(jointCount, Matrix4::IDENTITY);
// 	for (size_t jointIndex = 0; jointIndex < jointCount; jointCount++)
// 	{
// 		Matrix4 jointTransform = m_curves[jointIndex].Evaluate(time);
// 		skinningPose.GetTransforms(jointIndex) = jointTransform;
// 	}

	MatrixStack matStack;
	EvaluateSkinning(skinningPose, skeleton, skeleton->m_root, matStack, time);
}

void Animation::EvaluateSkinning(Pose& skinningPose, Skeleton * skeleton, SkeletonJoint* joint, MatrixStack& matStack, float const time) const
{
	size_t jointIndex = skeleton->GetJointIndex(joint);
	Matrix4 localTransform = m_curves[jointIndex].EvaluateMatrix(time);// *skeleton->GetJointLocalTransform(jointIndex);
	matStack.Push(localTransform);
	//Matrix4 evaluated = m_curves[jointIndex].Evaluate(time);
	skinningPose.GetTransform(jointIndex) = 
		skeleton->GetJointGlobalTransform(skeleton->GetJointIndex(joint)).GetInverse() 
		* matStack.Top();
	for (SkeletonJoint* childJoint : joint->m_children)
	{
		EvaluateSkinning(skinningPose, skeleton, childJoint, matStack, time);
		matStack.Pop();
	}
}

void Animation::EvaluatePose(Pose& pose, Skeleton * skeleton, float time) const
{
	time = fmod(time, m_duration);

	size_t jointCount = skeleton->GetJointCount();
	if(pose.m_transforms.size()!=jointCount)
		pose.m_transforms.resize(jointCount, Matrix4::IDENTITY);
	// 	for (size_t jointIndex = 0; jointIndex < jointCount; jointCount++)
	// 	{
	// 		Matrix4 jointTransform = m_curves[jointIndex].Evaluate(time);
	// 		skinningPose.GetTransforms(jointIndex) = jointTransform;
	// 	}

	MatrixStack matStack;
	EvaluatePose(pose, skeleton, skeleton->m_root, matStack, time);
}

void Animation::EvaluatePose(Pose& pose, Skeleton * skeleton, SkeletonJoint* joint, MatrixStack& matStack, float const time) const
{
	size_t jointIndex = skeleton->GetJointIndex(joint);
	Matrix4 localTransform;
	if (!m_curves[jointIndex].IsEmpty())
		localTransform = m_curves[jointIndex].EvaluateMatrix(time);
	else
		localTransform = skeleton->GetJointLocalTransform(jointIndex);
	Matrix4 flipXmat = Matrix4::CreateScale(Vector3(-1.f, 1.f, 1.f));
	localTransform = localTransform * flipXmat;
	//localTransform.Scale(Vector3(-1, 1, 1));
	

	Matrix4 jointLocalMat = skeleton->GetJointLocalTransform(jointIndex);

	//matStack.Push(jointLocalMat);
	
	Matrix4 poseRelative = localTransform * jointLocalMat.GetInverse();

// 	Matrix4 parentGlobalMatrix = skeleton->GetJointGlobalTransform(skeleton->GetJointIndex(joint->m_parent));
// 	Matrix4 poseTransform = parentGlobalMatrix.GetInverse()* matStack.Top();
	//pose.GetTransforms(jointIndex) = poseTransform;
	pose.GetTransform(jointIndex) = localTransform;
	for (SkeletonJoint* childJoint : joint->m_children)
	{
		EvaluatePose(pose, skeleton, childJoint, matStack, time);
		//matStack.Pop();
	}
}

Animation* Animation::GetOrLoad(const std::string& filePath)
{
	auto found = s_loadedAnimations.find(filePath);
	if (found != s_loadedAnimations.end())
		return found->second;
	//return Load(filePath);
	return nullptr;
}

#ifdef TOOLS_BUILD

Animation* Animation::LoadFromFBX(const std::string& filePath)
{
	Animation* animation = new Animation();
	bool result = FBXUtils::LoadAnimation(filePath, *animation);
	if (!result)
	{
		SAFE_DELETE(animation);
		return nullptr;
	}
	s_loadedAnimations[filePath] = animation;
	return animation;
}

#endif // TOOLS_BUILD