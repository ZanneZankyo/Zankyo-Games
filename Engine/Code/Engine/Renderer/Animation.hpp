#pragma once
#include <vector>
#include "AnimCurve.hpp"
#include "Engine\Math\MatrixStack.hpp"
#include <string>
#include <map>

class Skeleton;
class SkeletonJoint;
class Pose;

class Animation
{
public:

	Animation();

	std::vector<AnimCurve> m_curves;
	float m_duration;

	AnimCurve& GetCurveByIndex(size_t index);
	AnimCurve& GetCurveByName(const std::string& name);
	void EvaluateSkinning(Pose& pose, Skeleton * skeleton, float time) const;
	void EvaluateSkinning(Pose& skinningPose, Skeleton * skeleton, SkeletonJoint* joint, MatrixStack& matStack, float const time) const;
	void EvaluatePose(Pose& pose, Skeleton * skeleton, float time) const;
	void EvaluatePose(Pose& skinningPose, Skeleton * skeleton, SkeletonJoint* joint, MatrixStack& matStack, float const time) const;

	static Animation* GetOrLoad(const std::string& filePath);
	static Animation* Save(const std::string& filePath);
	static Animation* Load(const std::string& filePath);
	static Animation* LoadFromFBX(const std::string& filePath);

	static std::map<std::string, Animation*> s_loadedAnimations;
};

inline Animation::Animation()
	: m_curves()
	, m_duration(0)
{}