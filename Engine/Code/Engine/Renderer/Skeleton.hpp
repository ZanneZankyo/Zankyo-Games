#pragma once
#include <vector>
#include <string>
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Renderer\Motion.hpp"
#include <map>
#include "Animation.hpp"

class SkeletonJoint
{
public:
	std::string m_name;
	SkeletonJoint* m_parent;
	std::vector<SkeletonJoint*> m_children;
	
	Matrix4 m_transform; //World transform of the joint
	Matrix4 m_inverseTransform;

	static bool SaveToBuffer(std::vector<byte_t>& buffer, SkeletonJoint* joint);
	static SkeletonJoint* LoadToBuffer(const std::vector<byte_t>& buffer, size_t& bufferOffset, SkeletonJoint* parent = nullptr);
public:
	SkeletonJoint();
	SkeletonJoint(
		const std::string name, const Matrix4& localMat, SkeletonJoint* parent = nullptr);
	~SkeletonJoint();
	SkeletonJoint* GetJoint(unsigned int jointIndex);
	const SkeletonJoint* GetJoint(unsigned int jointIndex) const;
	SkeletonJoint* GetJoint(unsigned int jointIndex, unsigned int& currentIndex);
	const SkeletonJoint* GetJoint(unsigned int jointIndex, unsigned int& currentIndex) const;
};

class Skeleton
{
public:
	SkeletonJoint* m_root;
	std::vector<SkeletonJoint*> m_jointMap;

	Skeleton(SkeletonJoint* root = nullptr);
	~Skeleton();

	SkeletonJoint* GetJointParent(int jointIndex);
	int GetJointParentIndex(unsigned int jointIndex) const;

	void CalcJointMap();

	
	void Clear();
	void FlipX();
	std::string GetJointName(int jointIndex);

	void AddJoint(const std::string& name, const std::string& parentName, const Matrix4& transform);

	unsigned int CalcJointCount() const;
	unsigned int GetJointCount() const;
	static unsigned int EvaluateJointCount(SkeletonJoint* currentJoint);
	int GetJointIndex(const SkeletonJoint* joint) const;
	int GetJointIndex(std::string name) const;

	SkeletonJoint* GetJoint(int jointIndex);
	SkeletonJoint* GetJoint(const std::string& jointName);
	const SkeletonJoint* GetJoint(int jointIndex) const;
	const SkeletonJoint* GetJoint(const std::string& jointName) const;
	SkeletonJoint* EvaluateJoint(const std::string& jointName);
	SkeletonJoint* EvaluateJoint(int jointIndex);
	std::vector<SkeletonJoint*> GetAllJoints() const;
	static void GetAllJoints(std::vector<SkeletonJoint*>& joints, SkeletonJoint* currentJoint);
	Matrix4 GetJointGlobalTransform(unsigned int jointIndex);
	Matrix4 GetJointGlobalTransform(SkeletonJoint* joint);
	Matrix4 GetJointLocalTransform(unsigned int jointIndex);
	Matrix4 EvaluateJointTransform(const std::string& jointName);
	Matrix4 GetJointTransform(const std::string& jointName);

	static Skeleton* GetOrLoad(const std::string& filePath);
	static Skeleton* Load(const std::string& filePath);
	static bool Save(const std::string& filePath, const Skeleton* skeleton);

	static std::map<std::string, Skeleton*> s_loadedSkeletons;
};

class SkeletonInstance
{
public:
	Skeleton *m_skeleton; // skeleton we're applying poses to.  Used for heirachy information.
	Pose m_currentPose;  // my current skeletons pose.
	Pose m_currentSkinning;

						// Excercise:  How do we evaluate a global transform for this joint if the pose only contains 
						// local transforms?
						// Hint:  You will need "get_joint_parent" below.

	SkeletonInstance(Skeleton* skeleton);

	Matrix4 GetJointGlobalTransform(int jointIndex) const;
	void GetGlobalPose(Pose& pose, MatrixStack& matStack, SkeletonJoint* joint) const;

	std::vector<Matrix4> GetSkinningTransforms();

	// returns INVALID_INDEX (-1) if no parent exists.
	// Use the skeleton to get this information.
	int GetJointParent(int jointIndex) const;

	void ApplyMotion(Motion const *motion, float const time);
	void ApplyTimedMotion(Motion const* motion, float const time);
	void ApplyAnimation(Animation const* animation, float const time);
	void ApplyTimedMotionAdditive(Motion const* motion, float const time);
	void EvaluateSkinning();

	Matrix4 GetSocketTransform(const std::string& socketName) const;
	SkeletonJoint* GetJoint(const std::string& jointName);
	int GetJointIndex(SkeletonJoint* joint);
	void SetGlobalTransfrom(int jointIndex, Matrix4 globalMat);
};

inline Skeleton::Skeleton(SkeletonJoint * root)
	: m_root(root)
{
	if (!m_root)
		m_root = new SkeletonJoint();
}

inline Skeleton::~Skeleton()
{
	SAFE_DELETE(m_root);
}

inline SkeletonJoint::SkeletonJoint()
	: m_name("")
	, m_parent(nullptr)
	, m_transform(Matrix4::IDENTITY)
	, m_inverseTransform(Matrix4::IDENTITY)
{
}

inline SkeletonJoint::SkeletonJoint(
	const std::string name, const Matrix4& transform, 
	SkeletonJoint* parent /*= nullptr*/)
	: m_name(name)
	, m_parent(parent)
	, m_transform(transform)
	, m_inverseTransform(transform.GetInverse())
{
}

inline SkeletonJoint::~SkeletonJoint()
{
	for (auto childJoint : m_children)
		SAFE_DELETE(childJoint);
}

inline SkeletonJoint * SkeletonJoint::GetJoint(unsigned int jointIndex, unsigned int& currentIndex)
{
	if (jointIndex == currentIndex)
		return this;
	for (auto child : m_children)
	{
		currentIndex++;
		SkeletonJoint * result = child->GetJoint(jointIndex, currentIndex);
		if (result)
			return result;
	}
	return nullptr;
}

inline const SkeletonJoint * SkeletonJoint::GetJoint(unsigned int jointIndex, unsigned int& currentIndex) const
{
	if (jointIndex == currentIndex)
		return this;
	for (auto child : m_children)
	{
		currentIndex++;
		SkeletonJoint * result = child->GetJoint(jointIndex, currentIndex);
		if (result)
			return result;
	}
	return nullptr;
}

inline SkeletonInstance::SkeletonInstance(Skeleton* skeleton)
	: m_skeleton(skeleton)
{
	uint jointCount = skeleton->GetJointCount();
	m_currentPose.m_transforms.resize(jointCount);
	m_currentSkinning.m_transforms.resize(jointCount);
}