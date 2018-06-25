#pragma once
#include <string>
#include <vector>
#include "Engine\Math\Vertex3.hpp"
#include "Engine\Core\Timeline.hpp"
#include <map>

struct BlendShapeParameter
{
	Vector3 position;
	Vector3 normal;
	BlendShapeParameter(
		const Vector3 dPos = Vector3::ZERO,
		const Vector3 dNor = Vector3::ZERO
	)
		: position(dPos)
		, normal(dNor)
	{}
	BlendShapeParameter(const BlendShapeParameter& copy)
		: position(copy.position)
		, normal(copy.normal)
	{}
};

class BlendShapeChannel
{
public:
	std::string m_name;
	std::vector<BlendShapeParameter> m_points;

	BlendShapeChannel(const std::string& name = "") : m_name(name), m_points() {}
	BlendShapeChannel(const BlendShapeChannel& copy) : m_name(copy.m_name), m_points(copy.m_points) {}

	void AddPoint(const BlendShapeParameter& parameter)
	{	m_points.push_back(parameter);	}

};

class BlendShape
{
public:
	std::string m_name;
	std::vector<BlendShapeChannel> m_channels;
	std::vector<BlendShapeParameter> m_buffer;

	BlendShape(const std::string& name = "") : m_name(name), m_channels() {}
	BlendShape(const BlendShape& copy) : m_name(copy.m_name), m_channels(copy.m_channels) {}

	void AddChannel(const BlendShapeChannel& channel)
	{	m_channels.push_back(channel);	}
	BlendShapeParameter* GetBuffer();
	size_t GetBufferSize();
};

class BlendShapeAnimationChannel : public Timeline<float>
{};

class Mesh;

class BlendShapeAnimation
{
	
public:
	std::string m_meshName;
	float m_duration;
	std::vector<BlendShapeAnimationChannel> m_timelines;
public:
	BlendShapeAnimation(const std::string& meshName, float duration)
		: m_meshName(meshName)
		, m_duration(duration)
		, m_timelines() 
	{
		//s_loadedAnimations[m_meshName] = this;
	}
	static std::vector<BlendShapeAnimation *>* GetOrLoad(const std::string& filePath);
	static bool Save(const std::string& filePath, std::vector<BlendShapeAnimation *> motion);
	static std::vector<BlendShapeAnimation *>* Load(const std::string& filePath);
	static std::vector<BlendShapeAnimation *>* LoadFromFBX(const std::string& filePath, std::vector<Mesh*>* meshes);
	static std::map<std::string, std::vector<BlendShapeAnimation *>*> s_loadedAnimations;
	
};

struct BlendShapeConstantBufferType
{
	int channelSize;
	int vertexSize;
	int propertyCount;
	int padding;
};

class BlendShapeInstance
{
public:
	BlendShape* m_blendShape;
	std::vector<float> m_weights;
	BlendShapeConstantBufferType m_buffer;

	BlendShapeInstance(BlendShape* blendShape);
	bool SetBlendWeight(size_t shapeIndex, float weight);
	bool SetBlendWeight(const std::string& channelName, float weight);
	void ResetBlendWeight();
};