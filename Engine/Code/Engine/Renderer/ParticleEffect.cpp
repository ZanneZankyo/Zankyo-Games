#include "ParticleEffect.hpp"
#include "Engine\Core\Time.hpp"
#include "ParticleSystem.hpp"

ParticleEffect::ParticleEffect(const ParticleEffectDefinition* definition)
	: m_emitters()
	, m_definition(definition)
{
	for (auto emitterDef : definition->m_emitterDefs)
		m_emitters.push_back(new ParticleEmitter(emitterDef));
	ParticleSystem::GetInstance()->AddEffect(this);
}

ParticleEffect::ParticleEffect(const std::string& effectName)
	: m_emitters()
	, m_definition(ParticleEffectDefinition::GetDefinition(effectName))
{
	if (!m_definition)
		return;
	for (auto emitterDef : m_definition->m_emitterDefs)
		m_emitters.push_back(new ParticleEmitter(emitterDef));
	ParticleSystem::GetInstance()->AddEffect(this);
}

ParticleEffect::~ParticleEffect()
{
	DestroyImmediate();
}

void ParticleEffect::Play()
{
	for (auto emitter : m_emitters)
		emitter->Play();
}

void ParticleEffect::Stop()
{
	for (auto emitter : m_emitters)
		emitter->Stop();
}

void ParticleEffect::Reset()
{
	for (auto emitter : m_emitters)
		emitter->Reset();
}

void ParticleEffect::Prewarm()
{
	for (auto emitter : m_emitters)
		emitter->Prewarm();
}

void ParticleEffect::Update(int numOfInterationPerUpdate)
{
	float deltaSeconds = Time::deltaSeconds;
	static const float maxDeltaSeconds = 1.f / 60.f;
	if (deltaSeconds > maxDeltaSeconds)
		deltaSeconds = maxDeltaSeconds;

	float deltaSecondsPerUpdate = deltaSeconds / (float)numOfInterationPerUpdate;
	for (int updateCount = 0; updateCount < numOfInterationPerUpdate; updateCount++)
	{
		UpdateOnce(deltaSecondsPerUpdate);
	}
}

void ParticleEffect::UpdateOnce(float deltaSeconds)
{
	for (uint index = 0; index < m_emitters.size(); index++)
	{
		m_emitters[index]->Update(deltaSeconds);
		if (m_emitters[index]->DestroyOnFinished())
		{
			delete m_emitters[index];
			m_emitters.erase(m_emitters.begin() + index);
			index--;
		}
	}
}

bool ParticleEffect::IsFinished() const
{
	return m_emitters.empty();
}

void ParticleEffect::DestroyImmediate()
{
	for (auto emitter : m_emitters)
	{
		emitter->DestroyImmediate();
		delete emitter;
	}
	m_emitters.clear();
}
