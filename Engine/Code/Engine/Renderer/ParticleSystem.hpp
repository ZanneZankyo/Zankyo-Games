#pragma once 
#include <vector>
#include <map>
#include <string>
#include "ParticleEffect.hpp"

class ParticleSystem
{
public:
	std::vector<ParticleEffect*> m_effects;
	std::map<std::string, ParticleEffect*> m_savedEffects;
public:
	static void Update(int numOfInterationPerUpdate = 1);
	static void AddEffect(ParticleEffect* effect)
	{	
		if (s_instance)
			s_instance->m_effects.push_back(effect);	
	}
	static ParticleSystem* GetInstance()
	{	return s_instance;	}
	static void Start()
	{	
		s_instance = new ParticleSystem();	
		ParticleEffectDefinition::LoadDefinitions();
	}

private:
	ParticleSystem()
		: m_effects()
		, m_savedEffects()
	{}
	static ParticleSystem* s_instance;
};
