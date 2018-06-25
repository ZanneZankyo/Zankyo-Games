#include "ParticleEffectDefinition.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "..\File\FileSystem.hpp"

std::map<std::string, ParticleEffectDefinition*> ParticleEffectDefinition::s_definitions;

ParticleEffectDefinition::ParticleEffectDefinition(const std::string& filePath)
	: m_emitterDefs()
	, m_name()
{
	XMLElement element = XMLElement::OpenFile(filePath);
	ASSERT_OR_DIE(!element.IsEmpty(), "ParticleEffectDefinition(): Failed to load XML file!\n");
	m_name = element.GetAttribute("name");
	for (auto& child : element.GetChildNodes("Emitter"))
	{
		m_emitterDefs.push_back(new ParticleEmitterDefinition(child));
	}
	s_definitions[m_name] = this;
}

void ParticleEffectDefinition::LoadDefinitions()
{
	std::string filePath = "Data/XML/Particles/";
	std::string postFix = "*.xml";
	std::vector<std::string> fileNames = FileSystem::GetAllFileNamesInPath(filePath+postFix);
	for (auto& fileName : fileNames)
	{
		LoadDefinition(filePath + fileName);
	}
	/*LoadDefinition("Data/xml/particle.xml");
	LoadDefinition("Data/xml/ribbon.xml");*/
}

void ParticleEffectDefinition::LoadDefinition(const std::string& filePath)
{
	new ParticleEffectDefinition(filePath);
}

ParticleEffectDefinition* ParticleEffectDefinition::GetDefinition(const std::string& defName)
{
	auto found = s_definitions.find(defName);
	if (found != s_definitions.end())
		return found->second;
	return nullptr;
}

