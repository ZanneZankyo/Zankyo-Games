#pragma once
#include <vector>
#include "Particle.hpp"
#include "Engine\Math\AABB3.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Math\Sphere3.hpp"
#include "Engine\Tools\XMLUtils.hpp"
#include "Engine\RHI\Material.hpp"
#include "Engine\Core\RGBA.hpp"
#include "ParticleEffectDefinition.hpp"
#include "Engine\Math\Vertex3.hpp"

class ParticleEmitter
{
public:
	
	const ParticleEmitterDefinition* m_definition;

	bool m_isPlaying;
	bool m_isFirstParticleSpawned;

	float m_spawnCountThisFrame;

	float m_lifeSpam;

	Matrix4 m_transform;
	std::vector<Particle*> m_particles;

public:
	ParticleEmitter();
	ParticleEmitter(const ParticleEmitter& copy);
	ParticleEmitter(const ParticleEmitterDefinition* definition);
	~ParticleEmitter();

	void Stop();
	void Play();
	void Reset();
	void Prewarm();
	bool DestroyOnFinished();
	void DestroyImmediate();
	bool IsFinished() const;
	void Update(float deltaSeconds);
	void SortByDistance(const Vector3& centerPosition);
	bool GetVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const;
	bool GetRibbonVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const;
	bool GetDefaultVertices(std::vector<Vertex3>& vertices, const Vector3& cameraPosition) const;

};

