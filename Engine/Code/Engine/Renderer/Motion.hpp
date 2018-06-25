#pragma once
#include <vector>
#include "Engine\Math\Matrix4.hpp"
#include <string>
#include <map>

class Skeleton;
class SkeletonJoint;

class Pose
{
public:
	std::vector<Matrix4> m_transforms;
	//Local transforms of each joint
	Matrix4& GetTransform(unsigned int jointIndex);
	Matrix4 GetTransform(unsigned int jointIndex) const;
public:
	Pose(std::vector<Matrix4> transforms = std::vector<Matrix4>()) :m_transforms(transforms) {}
	Pose(const Pose& copy) :m_transforms(copy.m_transforms) {}
};

class Motion
{
public:
	std::string m_name;

	float m_framerate;
	float m_duration;

	std::vector<Pose> m_poses;
	std::vector<std::vector<std::pair<float, Matrix4>>> m_timedPose;
	std::vector<bool> m_isAnimated;
	std::vector<Pose> m_skinningMatrices;
	std::vector<std::vector<std::pair<float, Matrix4>>> m_timedSkinningPose;

	Motion();

	// Make sure poses is exactly long enough to encompass this time.
	float SetDuration(float time);
	Pose* GetPose(unsigned int frameIndex);
	bool GetIsAnimated(int jointIndex);
	void SetIsAnimated(int jointIndex, bool isAnimated = false);
	std::pair<float, Matrix4>* GetTimedLocalTransformAtJointAndTimeIndex(int jointIndex, int timeIndex);
	// framerate is 10hz, that means the time between two frames takes 1/10th of a second.
	// therefore if we have 3 frames in this motion, the total duration would be (3 - 1) * 0.1,
	// or 0.2 seconds long. 
	bool EvaluateTimed(Pose& out, float time) const;
	bool EvaluateTimedAdditive(Pose& out, float time) const;
	float GetDuration() const;
	unsigned int GetFrameCount() const;

	// This will fill out a pose with the evaluated pose at time
	// left as an excercise...
	bool Evaluate(Pose &out, float time) const;
	bool EvaluateSkinning(Pose &out, float time) const;
	bool EvaluateTimedSkinning(Pose& skinning, float time) const;

	Matrix4 GetTimedTransformAtTime(int jointIndex, float time) const;

	Matrix4 GetJointGlobalTransform(const Skeleton* skeleton, unsigned int jointIndex, const Pose& pose) const;
	void PreCalcSkinningMatrices(const Skeleton* skeleton);
	void PreCalcTimedSkinningMatrices(const Skeleton* skeleton);
	void PreCalcTimedSkinningMatrices(const Skeleton* skeleton, const SkeletonJoint* joint);
	std::vector<float> GetKeyFrames();

	void SetName(std::string& motionName);
	void SetFrameCount(unsigned int frameCount);

	size_t ByteVolumn() const;

	size_t GetPoseVolume() const;
	size_t GetTimedPoseVolume() const;

	static Motion* GetOrLoad(const std::string& filePath);
	static bool Save(const std::string& filePath, Motion* motion);
	static Motion* Load(const std::string& filePath);
	static Motion* LoadFromFBX(const std::string& filePath, Skeleton* skeleton);

	static std::map<std::string, Motion*> s_loadedMotions;
	
};
