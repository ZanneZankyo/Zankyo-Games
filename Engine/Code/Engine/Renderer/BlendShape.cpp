#include "BlendShape.hpp"
#include "Engine\Tools\fbx.hpp"
#include "Mesh.hpp"

std::map<std::string, std::vector<BlendShapeAnimation *>*> BlendShapeAnimation::s_loadedAnimations;

std::vector<BlendShapeAnimation *>* BlendShapeAnimation::GetOrLoad(const std::string& filePath)
{
	auto found = s_loadedAnimations.find(filePath);
	if (found != s_loadedAnimations.end())
		return found->second;
	//return Load(filePath);
	return nullptr;
}
#ifdef TOOLS_BUILD
std::vector<BlendShapeAnimation *>* BlendShapeAnimation::LoadFromFBX(const std::string& filePath, std::vector<Mesh*>* meshes)
{
	std::vector<BlendShapeAnimation *>* facialAnimations = new std::vector<BlendShapeAnimation *>();

	bool result = FBXUtils::LoadBlendShapeAnimation(filePath, *facialAnimations, *meshes);

	if (!result)
	{
		SAFE_DELETE(facialAnimations);
		return nullptr;
	}

	s_loadedAnimations[filePath] = facialAnimations;
	return facialAnimations;
}
#endif
BlendShapeInstance::BlendShapeInstance(BlendShape* blendShape) 
	: m_blendShape(blendShape), m_weights(), m_buffer()
{
	if (m_blendShape->m_channels.empty() || m_blendShape->m_channels[0].m_points.empty())
		return;
	size_t channelSize = m_blendShape->m_channels.size();
	size_t vertexSize = m_blendShape->m_channels[0].m_points.size();
	m_weights.resize(channelSize, 0);
	m_buffer.channelSize = (int)channelSize;
	m_buffer.vertexSize = vertexSize;
	m_buffer.propertyCount = 2; // blendShape only support delta position and normal for now
}

bool BlendShapeInstance::SetBlendWeight(size_t shapeIndex, float weight)
{
	if (shapeIndex >= m_blendShape->m_channels.size())
		return false;
	m_weights[shapeIndex] = weight;
	return true;
}

bool BlendShapeInstance::SetBlendWeight(const std::string& channelName, float weight)
{
	for (size_t channelIndex = 0; channelIndex < m_blendShape->m_channels.size(); channelIndex++)
	{
		BlendShapeChannel& channel = m_blendShape->m_channels[channelIndex];
		if (channel.m_name == channelName)
		{
			m_weights[channelIndex] = weight;
			return true;
		}
	}
	return false;
}

void BlendShapeInstance::ResetBlendWeight()
{
	for (float& weight : m_weights)
		weight = 0.f;
}

BlendShapeParameter* BlendShape::GetBuffer()
{
	if (!m_buffer.empty())
		return m_buffer.data();
	m_buffer.reserve(m_channels.size() * m_channels[0].m_points.size());
	for (auto& channel : m_channels)
		for (auto& point : channel.m_points)
			m_buffer.push_back(point);
	return m_buffer.data();
}

size_t BlendShape::GetBufferSize()
{
	if (!m_buffer.empty())
		return m_buffer.size();
	GetBuffer();
	return m_buffer.size();
}
