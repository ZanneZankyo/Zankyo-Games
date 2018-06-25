#pragma once
#include <string>
#include <map>
#include <vector>
#include "ParticleEmitterDefinition.hpp"
#include "Engine\Tools\XMLUtils.hpp"

class ParticleEffectDefinition
{
public:
	std::string m_name;
	std::vector<ParticleEmitterDefinition*> m_emitterDefs;

	ParticleEffectDefinition(const std::string& filePath);

	static void LoadDefinitions();
	static void LoadDefinition(const std::string& filePath);
	static ParticleEffectDefinition* GetDefinition(const std::string& defName);

private:
	static std::map<std::string, ParticleEffectDefinition*> s_definitions;
};