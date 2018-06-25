#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include "Engine\Core\Time.hpp"

Particle::Status::Status(float posX /*= 0*/, float posY /*= 0*/, float posZ /*= 0*/, float velX /*= 0*/, float velY /*= 0*/, float velZ /*= 0*/, float accX /*= 0*/, float accY /*= 0*/, float accZ /*= 0*/, float rotX /*= 0*/, float rotY /*= 0*/, float rotZ /*= 0*/, float aVelX /*= 0*/, float aVelY /*= 0*/, float aVelZ /*= 0 */)
	: m_position(posX, posY, posZ)
	, m_velocity(velX, velY, velZ)
	, m_acceleration(accX, accY, accZ)
	, m_rotation(rotX, rotY, rotZ)
	, m_angularVelocity(aVelX, aVelY, aVelZ)
{}

Particle::Status::Status(const Status& copy)
	: m_position(copy.m_position)
	, m_velocity(copy.m_velocity)
	, m_acceleration(copy.m_acceleration)
	, m_rotation(copy.m_rotation)
	, m_angularVelocity(copy.m_angularVelocity)
{}


Particle::Status::Status(const Vector3& pos, const Vector3& vel, const Vector3& acc /*= Vector3::ZERO*/, const Vector3& rot /*= Vector3::ZERO*/, const Vector3& angularVelocity /*= Vector3::ZERO*/)
	: m_position(pos)
	, m_velocity(vel)
	, m_acceleration(acc)
	, m_rotation(rot)
	, m_angularVelocity(angularVelocity)
{}

Particle::Status Particle::Status::operator*(float scale)
{
	Vector3 newPosition = m_position * scale;
	Vector3 newVelocity = m_velocity * scale;
	return Status(newPosition, newVelocity, m_acceleration, m_rotation, m_angularVelocity);
}

Particle::Status Particle::Status::operator-(const Status& statusToSubtract)
{
	Vector3 newPosition = m_position - statusToSubtract.m_position;
	Vector3 newVelocity = m_velocity - statusToSubtract.m_velocity;
	Vector3 newRotation = m_rotation - statusToSubtract.m_rotation;
	return Status(newPosition, newVelocity, m_acceleration, newRotation, m_angularVelocity);
}

Particle::Status Particle::Status::operator+(const Status& statusToAdd)
{
	Vector3 newPosition = m_position + statusToAdd.m_position;
	Vector3 newVelocity = m_velocity + statusToAdd.m_velocity;
	Vector3 newRotation = m_rotation + statusToAdd.m_rotation;
	return Status(newPosition, newVelocity, m_acceleration, newRotation, m_angularVelocity);
}

void Particle::UpdateStatus(float deltaSeconds)
{
	m_lifeSpam -= deltaSeconds;

	Status prevPrevPrevStatus = m_statusPrevPrev;
	Status prevPrevStatus = m_statusPrev;

	m_statusPrevPrev = m_statusPrev;
	m_statusPrev = m_status;

	m_status.m_acceleration = m_definition->m_constantAcceleration;

	Status deltaStatus(m_statusPrev.m_velocity, m_status.m_acceleration);

	//m_status = m_status + deltaStatus * deltaSeconds;
	//m_force = Vector3::ZERO;
	//

	//Vector3 k1, k2, k3, k4;

	Status k1(m_statusPrevPrev.m_velocity, m_statusPrevPrev.m_acceleration);
	Status k2(m_statusPrev.m_velocity, m_statusPrev.m_acceleration);
	Status k3(m_status.m_velocity, m_status.m_acceleration);

	m_status = m_status + (k3) * (deltaSeconds / 1.f);
}

Particle::Particle(const ParticleEmitterDefinition* definition, const ParticleEmitter* emitter)
	: m_status()
	, m_statusPrev()
	, m_statusPrevPrev()
	, m_lifeSpam(0.1f)
	, m_definition(definition)
	, m_emitter(emitter)
{
	Vector3 pos = definition->m_locationModule->NewLocation(); 
	pos = emitter->m_transform.TransformPosition(pos);
	Vector3 vel = definition->m_velocityModule->NewVelocity();
	vel = emitter->m_transform.TransformDirection(vel);
	m_status.m_position = pos;
	m_statusPrev.m_position = pos;
	m_statusPrevPrev.m_position = pos;
	m_status.m_velocity = vel;
	m_statusPrev.m_velocity = vel;
	m_statusPrevPrev.m_velocity = vel;
	m_lifeSpam = definition->m_lifeTime;
}
