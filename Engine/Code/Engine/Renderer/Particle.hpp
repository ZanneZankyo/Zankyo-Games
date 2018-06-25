#pragma once
#include "Engine\Math\Matrix4.hpp"
#include "ParticleEmitterDefinition.hpp"

class ParticleEmitter;
class Particle
{
	class Status
	{
	public:
		Vector3 m_position;
		Vector3 m_velocity;
		Vector3 m_acceleration;
		Vector3 m_rotation;
		Vector3 m_angularVelocity;

	public:
		Status(
			float posX = 0, float posY = 0, float posZ = 0,
			float velX = 0, float velY = 0, float velZ = 0,
			float accX = 0, float accY = 0, float accZ = 0,
			float rotX = 0, float rotY = 0, float rotZ = 0,
			float aVelX = 0, float aVelY = 0, float aVelZ = 0
		);
		Status(const Status& copy);
		Status(const Vector3& pos, const Vector3& vel, const Vector3& acc = Vector3::ZERO, 
			const Vector3& rot = Vector3::ZERO, const Vector3& angularVelocity = Vector3::ZERO);
		Status operator + (const Status& statusToAdd);
		Status operator - (const Status& statusToSubtract);
		Status operator * (float scale);
	};

public:

	Status m_status;
	Status m_statusPrev;
	Status m_statusPrevPrev;
	float m_lifeSpam;
	const ParticleEmitterDefinition* m_definition;
	const ParticleEmitter* m_emitter;

public:
	void UpdateStatus(float deltaSeconds);
	Particle(const ParticleEmitterDefinition* definition, const ParticleEmitter* emitter);
	//Particle(const Vector3& pos, const Vector3& vel, const Vector3& rot = Vector3::ZERO, const Vector3& aVel = Vector3::ZERO);
};