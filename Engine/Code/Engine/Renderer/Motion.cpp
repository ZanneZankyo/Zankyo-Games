#include "Motion.hpp"
#include "Skeleton.hpp"
#include "Engine\File\Persistence.hpp"
#include <algorithm>
#include "..\Math\MathUtils.hpp"
#include "..\Tools\fbx.hpp"
#include "..\Core\EngineBase.hpp"

std::map<std::string, Motion*> Motion::s_loadedMotions;

Motion::Motion()
	:m_name()
	,m_duration(0.f)
	,m_framerate(0.f)
	,m_poses()
	,m_skinningMatrices()
{

}

float Motion::SetDuration(float time)
{
	m_duration = time;
	return m_duration;
}

Pose * Motion::GetPose(unsigned int frameIndex)
{
	if(frameIndex < m_poses.size())
		return &m_poses[frameIndex];
	m_poses.resize(frameIndex+1);
	return &m_poses[frameIndex];
	//return nullptr;
}

bool Motion::GetIsAnimated(int jointIndex)
{
	if (jointIndex < (int)m_isAnimated.size())
		return m_isAnimated[jointIndex];
	m_isAnimated.resize(jointIndex + 1, false);
	return m_isAnimated[jointIndex];
}

void Motion::SetIsAnimated(int jointIndex, bool isAnimated /*= false*/)
{
	if (jointIndex >= (int)m_isAnimated.size())
		m_isAnimated.resize(jointIndex + 1, false);
	m_isAnimated[jointIndex] = isAnimated;
}

std::pair<float, Matrix4>* Motion::GetTimedLocalTransformAtJointAndTimeIndex(int jointIndex, int timeIndex)
{
	if (jointIndex >= (int)m_timedPose.size())
		m_timedPose.resize(jointIndex + 1);
	if (timeIndex >= (int)m_timedPose[jointIndex].size())
		m_timedPose[jointIndex].resize(timeIndex + 1);
	return &m_timedPose[jointIndex][timeIndex];
}

bool Motion::EvaluateTimed(Pose& out, float time) const
{
	time = fmod(time, m_duration);
	for (size_t jointIndex = 0; jointIndex < m_timedPose.size(); jointIndex++)
	{
		out.GetTransform(jointIndex) = GetTimedTransformAtTime(jointIndex, time);
	}

	return true;
}

bool Motion::EvaluateTimedAdditive(Pose& out, float time) const
{
	time = fmod(time, m_duration);
	for (size_t jointIndex = 0; jointIndex < m_timedPose.size(); jointIndex++)
	{
		if (!m_isAnimated[jointIndex])
			continue;
		out.GetTransform(jointIndex) = GetTimedTransformAtTime(jointIndex, time);
	}

	return true;
}

bool Motion::EvaluateTimedSkinning(Pose& skinning, float time) const
{
	time = fmod(time, m_duration);
	for (size_t jointIndex = 0; jointIndex < m_timedSkinningPose.size(); jointIndex++)
	{
		int frontIndex = -1;
		int endIndex = -1;
		bool evaluated = false;
		for (size_t keyIndex = 0; keyIndex < m_timedSkinningPose[jointIndex].size(); keyIndex++)
		{
			if (m_timedSkinningPose[jointIndex][keyIndex].first == time)
			{
				skinning.GetTransform(jointIndex) = m_timedSkinningPose[jointIndex][keyIndex].second;
				evaluated = true;
				break;
			}
			if (m_timedSkinningPose[jointIndex][keyIndex].first < time)
				frontIndex = keyIndex;
			else
				endIndex = keyIndex;
			if (frontIndex != -1 && endIndex != -1)
			{
				float fraction = RangeMap(
					time, m_timedSkinningPose[jointIndex][frontIndex].first,
					m_timedSkinningPose[jointIndex][endIndex].first,
					0.0, 1.0);
				skinning.GetTransform(jointIndex) =
					Interpolate(
						m_timedSkinningPose[jointIndex][frontIndex].second,
						m_timedSkinningPose[jointIndex][endIndex].second,
						fraction);
				evaluated = true;
				break;
			}
		}
		if (!evaluated)
		{
			if (frontIndex != -1)
				skinning.GetTransform(jointIndex) = m_timedSkinningPose[jointIndex][frontIndex].second;
			else if (endIndex != -1)
				skinning.GetTransform(jointIndex) = m_timedSkinningPose[jointIndex][endIndex].second;
			else
				skinning.GetTransform(jointIndex) = Matrix4::IDENTITY;
		}
	}


	return true;
}

Matrix4 Motion::GetTimedTransformAtTime(int jointIndex, float time) const
{
	int frontIndex = -1;
	int endIndex = -1;
	//bool evaluated = false;

	if (m_timedPose[jointIndex].empty())
		return Matrix4::IDENTITY;

	if (m_timedPose[jointIndex][0].first >= time)
		return m_timedPose[jointIndex][0].second;
	if (m_timedPose[jointIndex][m_timedPose[jointIndex].size()-1].first <= time)
		return m_timedPose[jointIndex][m_timedPose[jointIndex].size() - 1].second;

	/*for (size_t keyIndex = 0; keyIndex < m_timedPose[jointIndex].size(); keyIndex++)
	{
		if (m_timedPose[jointIndex][keyIndex].first == time)
		{
			return m_timedPose[jointIndex][keyIndex].second;
		}
		if (m_timedPose[jointIndex][keyIndex].first < time)
			frontIndex = keyIndex;
		else
			endIndex = keyIndex;
		if (frontIndex != -1 && endIndex != -1)
		{
			float fraction = RangeMap(
				time, m_timedPose[jointIndex][frontIndex].first,
				m_timedPose[jointIndex][endIndex].first,
				0.0, 1.0);
			return Interpolate(
					m_timedPose[jointIndex][frontIndex].second,
					m_timedPose[jointIndex][endIndex].second,
					fraction);
		}
	}*/

	int leftIndex = 0;
	int rightIndex = m_timedPose[jointIndex].size() - 1;
	int currentIndex = (leftIndex + rightIndex) / 2;
	while (true)
	{
		float leftTime = m_timedPose[jointIndex][currentIndex].first;
		float rightTime = m_timedPose[jointIndex][currentIndex + 1].first;
		if (leftTime <= time && time <= rightTime)
		{
			float fraction = RangeMap(
				time, leftTime,
				rightTime,
				0.0, 1.0);
			return Interpolate(
				m_timedPose[jointIndex][currentIndex].second,
				m_timedPose[jointIndex][currentIndex + 1].second,
				fraction);
		}
		else if (leftTime > time)
			rightIndex = currentIndex;
		else
			leftIndex = currentIndex;
		currentIndex = (leftIndex + rightIndex) / 2;
	}
	

	if (frontIndex != -1)
		return m_timedPose[jointIndex][frontIndex].second;
	else if (endIndex != -1)
		return m_timedPose[jointIndex][endIndex].second;
	else
		return Matrix4::IDENTITY;

}

float Motion::GetDuration() const
{
	return m_duration;
	//return (m_poses.size() - 1) / m_framerate;
}

unsigned int Motion::GetFrameCount() const
{
	return m_poses.size();
}

bool Motion::Evaluate(Pose &out, float time) const
{
	time = fmod(time, m_duration);
	int index = (int)(time * m_framerate);
	if (0 <= index&&index < (int)m_poses.size())
	{
		out.m_transforms = m_poses[index].m_transforms;
		return true;
	}
	return false;
}

bool Motion::EvaluateSkinning(Pose &out, float time) const
{
	time = fmod(time, m_duration);
	int index = (int)(time * m_framerate);
	if (0 <= index&&index < (int)m_skinningMatrices.size())
	{
		out.m_transforms = m_skinningMatrices[index].m_transforms;
		return true;
	}
	return false;
}

Matrix4 Motion::GetJointGlobalTransform(const Skeleton* skeleton, unsigned int jointIndex, const Pose& pose) const
{
	Matrix4 transform = Matrix4::IDENTITY;
	do
	{
		transform = transform * pose.m_transforms[jointIndex];//.ConcatenateTransform(m_currentPose.m_transforms[jointIndex]);
		jointIndex = skeleton->GetJointParentIndex(jointIndex);
	} while (jointIndex != -1);
	//Matrix4 transform = m_skeleton->GetJoint(jointIndex)->m_transform * m_currentPose.m_transforms[jointIndex];
	return transform;
}

void Motion::PreCalcSkinningMatrices(const Skeleton* skeleton)
{
	m_skinningMatrices.resize(m_poses.size());
	for (int poseIndex = 0; poseIndex < (int)m_poses.size(); poseIndex++)
	{
		Pose& pose = m_poses[poseIndex];
		Pose skinningMatrices;
		skinningMatrices.m_transforms.resize(pose.m_transforms.size(), Matrix4::IDENTITY);
		for (int jointIndex = 0; jointIndex < (int)pose.m_transforms.size(); jointIndex++)
		{
			Matrix4 skinningTransform = 
				skeleton->GetJoint(jointIndex)->m_transform.GetInverse() 
				* GetJointGlobalTransform(skeleton, jointIndex, pose);
			skinningMatrices.m_transforms[jointIndex] = (skinningTransform);
		}
		m_skinningMatrices[poseIndex] = skinningMatrices;
	}
}

void Motion::PreCalcTimedSkinningMatrices(const Skeleton* skeleton)
{
	m_timedSkinningPose.resize(m_timedPose.size());
	//SkeletonJoint* joint = skeleton->m_root;
	//PreCalcTimedSkinningMatrices(skeleton, joint);

	std::vector<float> keyFrames = GetKeyFrames();

	for (int jointIndex = 0; jointIndex < (int)m_timedPose.size(); jointIndex++)
	{
		for (size_t timeIndex = 0; timeIndex < keyFrames.size(); timeIndex++)
		{
			float time = keyFrames[timeIndex];

			int tempJointIndex = jointIndex;

			//Get Global Skinning Transform
			Matrix4 transform = Matrix4::IDENTITY;
			do
			{
				Matrix4 timedTransform = GetTimedTransformAtTime(tempJointIndex, time);
				transform = transform * timedTransform;
				tempJointIndex = skeleton->GetJointParentIndex(tempJointIndex);
			} while (tempJointIndex != -1);

			Matrix4 skinningTransform = skeleton->GetJoint(jointIndex)->m_transform.GetInverse() * transform;
			m_timedSkinningPose[jointIndex].push_back(std::pair<float, Matrix4>(time, skinningTransform));
		}
	}
}

void Motion::PreCalcTimedSkinningMatrices(const Skeleton* skeleton, const SkeletonJoint* joint)
{
	int jointIndex = skeleton->GetJointIndex(joint);
	for (size_t timeIndex = 0; timeIndex < m_timedPose[jointIndex].size(); timeIndex++)
	{
		float time = m_timedPose[jointIndex][timeIndex].first;

		int tempJointIndex = jointIndex;

		//Get Global Skinning Transform
		Matrix4 transform = Matrix4::IDENTITY;
		do
		{
			Matrix4 timedTransform = GetTimedTransformAtTime(tempJointIndex, time);
			transform = transform * timedTransform;
			tempJointIndex = skeleton->GetJointParentIndex(tempJointIndex);
		} while (tempJointIndex != -1);

		Matrix4 skinningTransform = skeleton->GetJoint(jointIndex)->m_transform.GetInverse() * transform;
		m_timedSkinningPose[jointIndex].push_back(std::pair<float, Matrix4>(time, skinningTransform));
	}
	for (auto& child : joint->m_children)
		PreCalcTimedSkinningMatrices(skeleton, child);
}

std::vector<float> Motion::GetKeyFrames()
{
	std::vector<float> keyFrames;
	for (auto& jointPoses : m_timedPose)
		for (auto& timePair : jointPoses)
			keyFrames.push_back(timePair.first);
	sort(keyFrames.begin(), keyFrames.end());
	static const float minInterval = 1.f / 60.f;
	for (int i = 0; i < (int)keyFrames.size(); i++)
		for (int j = i + 1; j < (int)keyFrames.size(); j++)
		{
			if (keyFrames[j] - keyFrames[i] < minInterval)
			{
				keyFrames.erase(keyFrames.begin() + j);
				j--;
			}
			else
				break;
		}
	return keyFrames;
}

void Motion::SetName(std::string& motionName)
{
	m_name = motionName;
}

void Motion::SetFrameCount(unsigned int frameCount)
{
	m_poses.resize(frameCount);
}

size_t Motion::ByteVolumn() const
{
	size_t volumn = 0;
	volumn += sizeof(m_name.size());
	volumn += sizeof(char) * m_name.size();
	volumn += sizeof(m_framerate);
	volumn += sizeof(m_duration);
	volumn += sizeof(m_poses.size());
	volumn += sizeof(Pose) * m_poses.size();
	volumn += sizeof(m_skinningMatrices.size());
	volumn += sizeof(Pose) * m_skinningMatrices.size();
	return volumn;
}

size_t Motion::GetPoseVolume() const
{
	if (m_poses.empty())
		return 0;
	return m_poses.size() * m_poses[0].m_transforms.size() * sizeof(Matrix4);
}

size_t Motion::GetTimedPoseVolume() const
{
	size_t sum = 0;
	for (auto& jointCurve : m_timedPose)
		sum += jointCurve.size() * (sizeof(float) + sizeof(Matrix4));
	return sum;
}

Motion* Motion::GetOrLoad(const std::string& filePath)
{
	//std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);
	auto found = s_loadedMotions.find(filePath);
	if (found != s_loadedMotions.end())
		return found->second;
	return Load(filePath);
}

bool Motion::Save(const std::string& filePath, Motion* motion)
{
	/*std::string m_name;

	float m_framerate;
	float m_duration;

	std::vector<Pose> m_poses;

	std::vector<Pose> m_skinningMatrices;*/

	std::vector<byte_t> buffer;
	buffer.reserve(motion->GetTimedPoseVolume());
	/*
	Persistence::PushBuffer(motion->m_name.data(), motion->m_name.size(), buffer, true);
	Persistence::PushBuffer(motion->m_framerate, buffer, true);
	Persistence::PushBuffer(motion->m_duration, buffer, true);
	Persistence::PushBuffer(motion->m_poses.size(), buffer, true);
	for (size_t poseIndex = 0; poseIndex < motion->m_poses.size(); poseIndex++)
	{
		Persistence::PushBuffer(motion->m_poses[poseIndex].m_transforms.data(), motion->m_poses[poseIndex].m_transforms.size(), buffer, true);
	}*/
	Persistence::PushBuffer(motion->m_name.data(), motion->m_name.size(), buffer, true);
	Persistence::PushBuffer(motion->m_framerate, buffer, true);
	Persistence::PushBuffer(motion->m_duration, buffer, true);
	Persistence::PushBuffer(motion->m_timedPose.size(), buffer, true);
	for (size_t jointIndex = 0; jointIndex < motion->m_timedPose.size(); jointIndex++)
	{
		Persistence::PushBuffer(motion->m_timedPose[jointIndex].size(), buffer, true);
		for (size_t keyIndex = 0; keyIndex < motion->m_timedPose[jointIndex].size(); keyIndex++)
		{
			Persistence::PushBuffer(motion->m_timedPose[jointIndex][keyIndex].first, buffer, true);
			Persistence::PushBuffer(motion->m_timedPose[jointIndex][keyIndex].second, buffer, true);
		}
	}
	Persistence::PushBuffer(motion->m_isAnimated.size(), buffer, true);
	for(bool isAnimated : motion->m_isAnimated)
		Persistence::PushBuffer(isAnimated, buffer, true);

	Persistence::SaveBufferToBinaryFile(filePath, buffer);
	return true;
}

Motion* Motion::Load(const std::string& filePath)
{
	Motion* motion = new Motion();
	std::vector<byte_t> buffer;
	if (!Persistence::LoadBinaryFileToBuffer(filePath, buffer))
		return nullptr;
	size_t bufferOffset = 0;
	/*motion->m_name = Persistence::ReadBufferAsString(buffer, bufferOffset);
	motion->m_framerate = Persistence::ReadBuffer<float>(buffer, bufferOffset);
	motion->m_duration = Persistence::ReadBuffer<float>(buffer, bufferOffset);
	motion->m_poses.resize(Persistence::ReadBuffer<int>(buffer, bufferOffset), Pose());
	for (size_t poseIndex = 0; poseIndex < motion->m_poses.size(); poseIndex++)
	{
		motion->m_poses[poseIndex].m_transforms = Persistence::ReadBufferArray<Matrix4>(buffer, bufferOffset);
	}*/

	motion->m_name = Persistence::ReadBufferAsString(buffer, bufferOffset);
	motion->m_framerate = Persistence::ReadBuffer<float>(buffer, bufferOffset);
	motion->m_duration = Persistence::ReadBuffer<float>(buffer, bufferOffset);
	motion->m_timedPose.resize(Persistence::ReadBuffer<uint>(buffer, bufferOffset));
	for (size_t jointIndex = 0; jointIndex < motion->m_timedPose.size(); jointIndex++)
	{
		uint keys = Persistence::ReadBuffer<uint>(buffer, bufferOffset);
		for (size_t keyIndex = 0; keyIndex < keys; keyIndex++)
		{
			float time = Persistence::ReadBuffer<float>(buffer, bufferOffset);
			Matrix4 transform = Persistence::ReadBuffer<Matrix4>(buffer, bufferOffset);
			motion->m_timedPose[jointIndex].push_back(std::pair<float, Matrix4>(time, transform));
		}
	}
	motion->m_isAnimated.resize(Persistence::ReadBuffer<uint>(buffer, bufferOffset),false);
	for (size_t index = 0; index < motion->m_isAnimated.size(); index++)
		motion->m_isAnimated[index] = Persistence::ReadBuffer<bool>(buffer, bufferOffset);
	s_loadedMotions[filePath] = motion;
	return motion;
}

#ifdef TOOLS_BUILD
Motion* Motion::LoadFromFBX(const std::string& filePath, Skeleton* skeleton)
{

	Motion* motion = new Motion();

	bool result = FBXUtils::LoadTimedMotion(motion, skeleton, filePath.c_str());

	if (!result)
	{
		SAFE_DELETE(motion);
		return nullptr;
	}
	
	s_loadedMotions[filePath] = motion;
	return motion;
	
}
#endif

Matrix4& Pose::GetTransform(unsigned int jointIndex)
{
	if (jointIndex < m_transforms.size())
		return m_transforms[jointIndex];
	m_transforms.resize(jointIndex + 1, Matrix4::IDENTITY);
	return m_transforms[jointIndex];
}

Matrix4 Pose::GetTransform(unsigned int jointIndex) const
{
	if (jointIndex < m_transforms.size())
		return m_transforms[jointIndex];
	return Matrix4::IDENTITY;
}
