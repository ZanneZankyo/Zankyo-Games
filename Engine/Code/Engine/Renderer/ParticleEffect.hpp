#pragma once 
#include <vector>
#include <string>
#include "ParticleEmitter.hpp"
#include "ParticleEffectDefinition.hpp"
#include "Engine\Math\Vertex3.hpp"

class ParticleEffect
{
public:

	std::vector<ParticleEmitter*> m_emitters;
	const ParticleEffectDefinition* m_definition;

public:

	ParticleEffect(const ParticleEffectDefinition* definition);
	ParticleEffect(const std::string& effectName);
	~ParticleEffect();

	void Play();
	void Stop();
	void Reset();
	void Prewarm();
	void Update(int numOfInterationPerUpdate);
	void UpdateOnce(float deltaSeconds);
	bool IsFinished() const;
	void DestroyImmediate();

};