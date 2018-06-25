#include "Skeleton.hpp"
#include <queue>
#include "Engine\File\Persistence.hpp"
#include <algorithm>
#include "Engine\Core\Time.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Tools\fbx.hpp"
#include "Engine\Core\Console.hpp"

std::map<std::string, Skeleton*> Skeleton::s_loadedSkeletons;

Matrix4 Skeleton::GetJointTransform(const std::string& jointName)
{
	SkeletonJoint* joint = GetJoint(jointName);
	if (!joint)
		return Matrix4::IDENTITY;
	return joint->m_transform;
}

Matrix4 Skeleton::EvaluateJointTransform(const std::string& jointName)
{
	SkeletonJoint* joint = EvaluateJoint(jointName);
	if (!joint)
		return Matrix4::IDENTITY;
	return joint->m_transform;
}

Skeleton* Skeleton::GetOrLoad(const std::string& filePath)
{
	//std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
	auto found = s_loadedSkeletons.find(filePath);
	if (found != s_loadedSkeletons.end())
		return found->second;
	return Load(filePath);
}

Skeleton* Skeleton::Load(const std::string& filePath)
{

	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');
	Skeleton* skeleton = new Skeleton();
	if (fileAndPostFix.second == "fbx")
	{
#ifdef TOOLS_BUILD
		bool result = FBXUtils::LoadSkeleton(skeleton, filePath);
		if (!result)
		{
			SAFE_DELETE(skeleton);
			return nullptr;
		}
#else
		Console::Log("Failed to load skeleton from " + filePath + ": not a tools build.");
		SAFE_DELETE(skeleton);
		return nullptr;
#endif // TOOLS_BUILD
	}
	else
	{
		std::vector<byte_t> buffer;
		bool loadFileResult = Persistence::LoadBinaryFileToBuffer(filePath, buffer);
		if (!loadFileResult)
			return false;
		size_t bufferOffset = 0;
		skeleton->m_root = SkeletonJoint::LoadToBuffer(buffer, bufferOffset);
		skeleton->CalcJointMap();
	}
	s_loadedSkeletons[filePath] = skeleton;
	return skeleton;
}

bool Skeleton::Save(const std::string& filePath, const Skeleton* skeleton)
{
	std::vector<byte_t> buffer;

	SkeletonJoint::SaveToBuffer(buffer, skeleton->m_root);

	return Persistence::SaveBufferToBinaryFile(filePath, buffer);
}

void Skeleton::Clear()
{
	SAFE_DELETE(m_root);
}

void Skeleton::FlipX()
{
	std::vector<SkeletonJoint*> joints;
	GetAllJoints(joints, m_root);
	for (auto joint : joints)
	{
		joint->m_transform.Scale(Vector3(-1.f, 1.f, 1.f));
	}
}

std::string Skeleton::GetJointName(int jointIndex)
{
	SkeletonJoint *joint = GetJoint(jointIndex);
	return joint->m_name;
}

void Skeleton::AddJoint(const std::string& name, const std::string& parentName, const Matrix4& transform)
{
	SkeletonJoint* parent = nullptr;
	if(!parentName.empty())
		parent = EvaluateJoint(parentName);
	if (parent)
		parent->m_children.push_back(new SkeletonJoint(name, transform, parent));
	else
		m_root = new SkeletonJoint(name, transform, parent);
}

unsigned int Skeleton::CalcJointCount() const
{
	return EvaluateJointCount(m_root);
}

unsigned int Skeleton::GetJointCount() const
{
	return m_jointMap.size();
}

unsigned int Skeleton::EvaluateJointCount(SkeletonJoint* currentJoint)
{
	unsigned int count = 0;
	if (!currentJoint)
		return count;
	count++;

	//if(!currentJoint->m_children.empty())
		for (auto child : currentJoint->m_children)
			count += EvaluateJointCount(child);

	return count;
}

int Skeleton::GetJointIndex(std::string name) const
{
	unsigned int JointCount = GetJointCount();
	for (int i = 0; i < (int)JointCount; i++)
		if (GetJoint(i)->m_name == name)
			return i;
	return -1;
}

int Skeleton::GetJointIndex(const SkeletonJoint* joint) const
{
	/*if (!joint)
		return -1;
	unsigned int jointCount = GetJointCount();
	for (int i = 0; i < jointCount; i++)
		if (GetJoint(i) == joint)
			return i;
	return -1;*/
	for (int i = 0; i < (int)m_jointMap.size(); i++)
		if (m_jointMap[i] == joint)
			return i;
	return -1;
}

SkeletonJoint * Skeleton::GetJoint(const std::string& jointName)
{
	/*std::queue<SkeletonJoint*> jointQueue;
	if (!m_root)
		return nullptr;
	jointQueue.push(m_root);
	while (!jointQueue.empty())
	{
		SkeletonJoint* joint = jointQueue.front();
		jointQueue.pop();
		if (!joint)
			continue;
		if (joint->m_name == jointName)
			return joint;
		for (auto childJoint : joint->m_children)
			if(childJoint)
				jointQueue.push(childJoint);
	}
	return nullptr;*/
	for (auto joint : m_jointMap)
		if (joint->m_name == jointName)
			return joint;
	return nullptr;
}

SkeletonJoint* Skeleton::GetJoint(int jointIndex)
{
	//return m_root->GetJoint(jointIndex);
	if (0 <= jointIndex && jointIndex < (int)m_jointMap.size())
		return m_jointMap[jointIndex];
	return nullptr;
}

const SkeletonJoint* Skeleton::GetJoint(int jointIndex) const
{
	if(0 <= jointIndex && jointIndex < (int)m_jointMap.size())
		return m_jointMap[jointIndex];
	return nullptr;
}

const SkeletonJoint* Skeleton::GetJoint(const std::string& jointName) const
{
	for (auto joint : m_jointMap)
		if (joint->m_name == jointName)
			return joint;
	return nullptr;
}

SkeletonJoint* Skeleton::EvaluateJoint(int jointIndex)
{
	return m_root->GetJoint(jointIndex);
}

SkeletonJoint* Skeleton::EvaluateJoint(const std::string& jointName)
{
	std::queue<SkeletonJoint*> jointQueue;
	if (!m_root)
	return nullptr;
	jointQueue.push(m_root);
	while (!jointQueue.empty())
	{
		SkeletonJoint* joint = jointQueue.front();
		jointQueue.pop();
		if (!joint)
			continue;
		if (joint->m_name == jointName)
			return joint;
		for (auto childJoint : joint->m_children)
			if (childJoint)
				jointQueue.push(childJoint);
	}
	return nullptr;
}

std::vector<SkeletonJoint*> Skeleton::GetAllJoints() const
{
	std::vector<SkeletonJoint*> joints;
	GetAllJoints(joints, m_root);
	return joints;
}

void Skeleton::GetAllJoints(std::vector<SkeletonJoint*>& joints, SkeletonJoint * currentJoint)
{
	if (!currentJoint) return;
	joints.push_back(currentJoint);
	for (auto child : currentJoint->m_children)
		GetAllJoints(joints, child);
}


Matrix4 Skeleton::GetJointGlobalTransform(unsigned int jointIndex)
{
	if (jointIndex == -1)
		return Matrix4::IDENTITY;
	return GetJoint(jointIndex)->m_transform;
}

Matrix4 Skeleton::GetJointGlobalTransform(SkeletonJoint* joint)
{
	int jointIndex = GetJointIndex(joint);
	if (jointIndex == -1)
		return Matrix4::IDENTITY;
	return GetJointGlobalTransform(jointIndex);
}

Matrix4 Skeleton::GetJointLocalTransform(unsigned int jointIndex)
{
	if (jointIndex == -1)
		return Matrix4::IDENTITY;
	SkeletonJoint* joint = GetJoint(jointIndex);
	if (!joint->m_parent)
		return joint->m_transform;
	return joint->m_transform * joint->m_parent->m_transform.GetInverse();
}

SkeletonJoint* Skeleton::GetJointParent(int jointIndex)
{
	return GetJoint(jointIndex)->m_parent;
}

int Skeleton::GetJointParentIndex(unsigned int jointIndex) const
{
	return GetJointIndex(GetJoint(jointIndex)->m_parent);
}

void Skeleton::CalcJointMap()
{
	m_jointMap.clear();
	//m_jointMap = GetAllJoints();
	int jointCount = CalcJointCount();
	for (int jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		SkeletonJoint* joint = EvaluateJoint(jointIndex);
		m_jointMap.push_back(joint);
	}
}

Matrix4 SkeletonInstance::GetJointGlobalTransform(int jointIndex) const
{
	//return m_skeleton->GetJoint(jointIndex)->m_transform;
		//m_currentPose.m_transforms[jointIndex];
	Matrix4 transform = Matrix4::IDENTITY;
	while (jointIndex != -1)
	{
		transform = transform * m_currentPose.m_transforms[jointIndex];
		jointIndex = GetJointParent(jointIndex);
	} 
	//Matrix4 transform = m_skeleton->GetJoint(jointIndex)->m_transform * m_currentPose.m_transforms[jointIndex];
	return transform;
}

void SkeletonInstance::GetGlobalPose(Pose& pose, MatrixStack& matStack, SkeletonJoint* joint) const
{
	int jointIndex = m_skeleton->GetJointIndex(joint);
	matStack.PushDirect(m_currentPose.GetTransform(jointIndex) * matStack.Top());
	pose.m_transforms.reserve(m_currentPose.m_transforms.size());
	pose.GetTransform(jointIndex) = matStack.Top();
	
	for (auto& child : joint->m_children)
	{
		GetGlobalPose(pose, matStack, child);
		matStack.Pop();
	}
}

std::vector<Matrix4> SkeletonInstance::GetSkinningTransforms()
{
	float start = (float)GetCurrentTimeSeconds();
	Pose globalPose;
	MatrixStack matStack;
	GetGlobalPose(globalPose, matStack, m_skeleton->m_root);
	std::vector<Matrix4> skinningTransforms;
	float evalPose = (float)GetCurrentTimeSeconds() - start;
	UNUSED(evalPose);
	skinningTransforms.reserve(m_currentPose.m_transforms.size());
	for (int jointIndex = 0; jointIndex < (int)m_currentPose.m_transforms.size(); jointIndex++)
	{
		//int parentJoint = GetJointParent(jointIndex);
		Matrix4 skinningTransform;

		skinningTransform =
			m_skeleton->GetJoint(jointIndex)->m_inverseTransform * globalPose.GetTransform(jointIndex);
		skinningTransforms.push_back(skinningTransform);
	}
	float end = (float)GetCurrentTimeSeconds() - start;
	UNUSED(end);
	return skinningTransforms;
}

int SkeletonInstance::GetJointParent(int jointIndex) const
{
	SkeletonJoint* parent = m_skeleton->GetJointParent(jointIndex);
	if (!parent)
		return -1;
	return m_skeleton->GetJointIndex(parent->m_name);
}

void SkeletonInstance::ApplyMotion(Motion const *motion, float const time)
{
	motion->Evaluate(m_currentPose, time);
	//m_currentSkinning.m_transforms = GetSkinningTransforms();
	//motion->EvaluateSkinning(m_currentSkinning, time);
	//m_currentSkinning.m_transforms = GetSkinningTransforms();
}

void SkeletonInstance::ApplyTimedMotion(Motion const* motion, float const time)
{
	motion->EvaluateTimed(m_currentPose, time);
}

void SkeletonInstance::ApplyAnimation(Animation const* animation, float const time)
{
	animation->EvaluatePose(m_currentPose, m_skeleton, time);
	//m_currentSkinning.m_transforms = GetSkinningTransforms();
	//animation->EvaluateSkinning(m_currentSkinning, m_skeleton, time);
}

void SkeletonInstance::ApplyTimedMotionAdditive(Motion const* motion, float const time)
{
	motion->EvaluateTimedAdditive(m_currentPose, time);
}

void SkeletonInstance::EvaluateSkinning()
{
	m_currentSkinning.m_transforms = GetSkinningTransforms();
}

Matrix4 SkeletonInstance::GetSocketTransform(const std::string& socketName) const
{
	int jointIndex = m_skeleton->GetJointIndex(socketName);
	if (0 <= jointIndex && jointIndex < (int)m_skeleton->GetJointCount())
		return GetJointGlobalTransform(jointIndex);
		//return m_currentPose.GetTransforms(jointIndex);
	return Matrix4::IDENTITY;
}

SkeletonJoint* SkeletonInstance::GetJoint(const std::string& jointName)
{
	if (!m_skeleton)
		return nullptr;
	return m_skeleton->GetJoint(jointName);
}

int SkeletonInstance::GetJointIndex(SkeletonJoint* joint)
{
	if (!m_skeleton)
		return -1;
	return m_skeleton->GetJointIndex(joint);
}

void SkeletonInstance::SetGlobalTransfrom(int jointIndex, Matrix4 globalMat)
{
	int currentJoint = GetJointParent(jointIndex);
	while (currentJoint != -1)
	{
		globalMat = m_currentPose.m_transforms[currentJoint].GetInverse() * globalMat;
		currentJoint = GetJointParent(currentJoint);
	}
	//Matrix4 transform = m_skeleton->GetJoint(jointIndex)->m_transform * m_currentPose.m_transforms[jointIndex];
	m_currentPose.m_transforms[jointIndex] = globalMat;
}

bool SkeletonJoint::SaveToBuffer(std::vector<byte_t>& buffer, SkeletonJoint* joint)
{
	Persistence::PushBuffer(joint->m_name.data(),joint->m_name.size(), buffer);
	Persistence::PushBuffer(joint->m_transform, buffer);
	Persistence::PushBuffer(joint->m_children.size(), buffer);
	for (size_t childIndex = 0; childIndex < joint->m_children.size(); childIndex++)
	{
		SaveToBuffer(buffer, joint->m_children[childIndex]);
	}
	return true;
}

SkeletonJoint* SkeletonJoint::LoadToBuffer(const std::vector<byte_t>& buffer, size_t& bufferOffset, SkeletonJoint* parent)
{
	SkeletonJoint* joint = new SkeletonJoint();
	//std::vector<char> stringArray = Persistence::ReadBufferArray<char>(buffer, bufferOffset);
	joint->m_name = Persistence::ReadBufferAsString(buffer, bufferOffset);//std::string(stringArray.data(),stringArray.size());
	joint->m_transform = Persistence::ReadBuffer<Matrix4>(buffer, bufferOffset);
	joint->m_children.resize(Persistence::ReadBuffer<size_t>(buffer, bufferOffset), nullptr);
	joint->m_parent = parent;
	joint->m_inverseTransform = joint->m_transform.GetInverse();
	for (size_t childIndex = 0; childIndex < joint->m_children.size(); childIndex++)
	{
		joint->m_children[childIndex] = LoadToBuffer(buffer, bufferOffset, joint);
	}
	return joint;
}

SkeletonJoint* SkeletonJoint::GetJoint(unsigned int jointIndex)
{
	unsigned int index = 0;
	return GetJoint(jointIndex, index);
}

const SkeletonJoint* SkeletonJoint::GetJoint(unsigned int jointIndex) const
{
	unsigned int index = 0;
	return GetJoint(jointIndex, index);
}
