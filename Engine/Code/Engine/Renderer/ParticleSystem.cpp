#include "ParticleSystem.hpp"

ParticleSystem* ParticleSystem::s_instance = nullptr;

void ParticleSystem::Update(int numOfInterationPerUpdate /*= 1*/)
{
	if (!s_instance)
		return;
	for (size_t index = 0; index < s_instance->m_effects.size(); index++)
	{
		ParticleEffect* effect = s_instance->m_effects[index];
		effect->Update(numOfInterationPerUpdate);
		if (effect->IsFinished())
		{
			delete effect;
			s_instance->m_effects.erase(s_instance->m_effects.begin() + index);
			index--;
		}
	}
}
