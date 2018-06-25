#pragma once
#include <string>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/RGBA.hpp"
#include "Engine/RHI/Material.hpp"
#include "Engine/Tools/XMLUtils.hpp"
#include "Engine/Core/Timeline.hpp"

class ParticleEmitterLocationModule
{
public:
	virtual Vector3 NewLocation() = 0;
};

class ParticleEmitterLocationModuleBox : public ParticleEmitterLocationModule
{
public:
	AABB3 m_box;
	ParticleEmitterLocationModuleBox(const AABB3& box = AABB3::ZERO_TO_ONE) : m_box(box) {}
	ParticleEmitterLocationModuleBox(const XMLElement& element);
	virtual Vector3 NewLocation() override
	{
		return Vector3(
			GetRandomFloatInRange(m_box.mins.x, m_box.maxs.x),
			GetRandomFloatInRange(m_box.mins.y, m_box.maxs.y),
			GetRandomFloatInRange(m_box.mins.z, m_box.maxs.z)
		);
	}
};

class ParticleEmitterLocationModuleSphere : public ParticleEmitterLocationModule
{
public:
	Sphere3 m_maxSphere;
	Sphere3 m_minSphere;
	ParticleEmitterLocationModuleSphere(float minR = 0.f, float maxR = 1.f, const Vector3& position = Vector3::ZERO)
		: m_minSphere(position, minR)
		, m_maxSphere(position, maxR)
	{}
	ParticleEmitterLocationModuleSphere(const XMLElement& element);
	virtual Vector3 NewLocation() override
	{
		float radius = GetRandomFloatInRange(m_minSphere.radius, m_maxSphere.radius);
		Vector3 position(m_maxSphere.position);
		do
		{
			position = m_minSphere.position +
				Vector3(
					GetRandomFloatInRange(-radius, radius),
					GetRandomFloatInRange(-radius, radius),
					GetRandomFloatInRange(-radius, radius)
				) * 0.5f;
		} while (!m_maxSphere.IsPointInside(position) || m_minSphere.IsPointInside(position));
		return position;
	}
};

class ParticleEmitterVelocityModule
{
public:
	virtual Vector3 NewVelocity() = 0;
};

class ParticleEmitterVelocityModuleBox : public ParticleEmitterVelocityModule
{
public:
	AABB3 m_box;
	ParticleEmitterVelocityModuleBox(const Vector3& vel = Vector3::ZERO)
		: m_box(vel, vel)
	{}
	ParticleEmitterVelocityModuleBox(const Vector3& minVel, const Vector3& maxVel)
		: m_box(minVel, maxVel)
	{}
	ParticleEmitterVelocityModuleBox(const XMLElement& element);
	virtual Vector3 NewVelocity() override
	{
		return Vector3(
			GetRandomFloatInRange(m_box.mins.x, m_box.maxs.x),
			GetRandomFloatInRange(m_box.mins.y, m_box.maxs.y),
			GetRandomFloatInRange(m_box.mins.z, m_box.maxs.z)
		);
	};
};

class ParticleEmitterVelocityModuleLine : public ParticleEmitterVelocityModule
{
public:
	Vector3 m_minVelocity;
	Vector3 m_maxVelocity;
	ParticleEmitterVelocityModuleLine(const Vector3& vel = Vector3::ZERO)
		: m_minVelocity(vel)
		, m_maxVelocity(vel)
	{}
	ParticleEmitterVelocityModuleLine(const Vector3& minVel, const Vector3& maxVel)
		: m_minVelocity(minVel)
		, m_maxVelocity(maxVel)
	{}
	ParticleEmitterVelocityModuleLine(const XMLElement& element);
	virtual Vector3 NewVelocity() override
	{
		float fraction = GetRandomFloatZeroToOne();
		return Interpolate(m_minVelocity, m_maxVelocity, fraction);
	};
};

class ParticleEmitterVelocityModuleCone : public ParticleEmitterVelocityModule
{
public:
	Vector3 m_direction;
	float	m_minSpeed;
	float	m_maxSpeed;
	float	m_angle;
	ParticleEmitterVelocityModuleCone(const Vector3& dir = Vector3::Y_AXIS, float minSpd = 0.f, float maxSpd = 1.f, float angle = 30.f)
		: m_direction(dir.GetNormalize())
		, m_minSpeed(minSpd)
		, m_maxSpeed(maxSpd)
		, m_angle(angle)
	{}
	ParticleEmitterVelocityModuleCone(const XMLElement& element);
	virtual Vector3 NewVelocity() override
	{
		float speed = GetRandomFloatInRange(m_minSpeed, m_maxSpeed);
		//float angle = GetRandomFloatInRange(0.f, m_angle);
		Vector3 leftAxis;
		if (m_direction != Vector3::Y_AXIS)
			leftAxis = CrossProduct(Vector3::Y_AXIS, m_direction);
		else
			leftAxis = Vector3::X_AXIS;

		Vector3 upAxis = -CrossProduct(leftAxis, m_direction);
		float radius = SinDegrees(m_angle) * speed;
		float isNegative = radius >= 0 ? 1.f : -1.f;
		Vector2 delta;
		Disc2 circle(Vector2::ZERO, fabs(radius));
		do
		{
			delta = Vector2(
				GetRandomFloatInRange(-radius, radius),
				GetRandomFloatInRange(-radius, radius)
			);
		} while (!circle.IsPointInside(delta));
		Vector3 velocity = isNegative * speed * CosDegrees(m_angle) * m_direction + delta.x * leftAxis + delta.y * upAxis;
		return velocity;
	}
};

class ParticleEmitterVelocityModuleSphere : public ParticleEmitterVelocityModule
{
public:
	Sphere3 m_maxSphere;
	Sphere3 m_minSphere;
	ParticleEmitterVelocityModuleSphere(float minR = 0.f, float maxR = 1.f, const Vector3& position = Vector3::ZERO)
		: m_minSphere(position, minR)
		, m_maxSphere(position, maxR)
	{}
	ParticleEmitterVelocityModuleSphere(const XMLElement& element);
	virtual Vector3 NewVelocity() override
	{
		float radius = GetRandomFloatInRange(m_minSphere.radius, m_maxSphere.radius);
		Vector3 position(m_maxSphere.position);
		do
		{
			position = m_minSphere.position +
				Vector3(
					GetRandomFloatInRange(-radius, radius),
					GetRandomFloatInRange(-radius, radius),
					GetRandomFloatInRange(-radius, radius)
				) * 0.5f;
		} while (!m_maxSphere.IsPointInside(position) || m_minSphere.IsPointInside(position));
		return position;
	}
};


class ParticleEmitterDefinition
{
public:

	enum ParticleType
	{
		DEFAULT,
		RIBBON,
		MESH
	};

	enum LocationModuleType
	{
		LOCATION_BOX,
		LOCATION_SPHERE,
	};

	enum VelocityModuleType
	{
		VELOCITY_BOX,
		VELOCITY_LINE,
		VELOCITY_SPHERE,
		VELOCITY_CONE
	};

	float m_lifeTime; // particle lifetime
	float m_spawnTime; // seconds before it stop spawning particle

	ParticleType m_particleType;

	LocationModuleType m_locationModuleType;
	ParticleEmitterLocationModule* m_locationModule;

	VelocityModuleType m_velocityModuleType;
	ParticleEmitterVelocityModule* m_velocityModule;

	Vector3 m_constantAcceleration;

	bool m_doesPrewarm;

	uint m_burst; // initial burst
	uint m_spawn; // spawn per second

	//Rgba m_startColor;
	//Rgba m_endColor;
	Timeline<Rgba> m_colorByLife;

	//float m_startSize;
	//float m_endSize;
	Timeline<float> m_sizeByLife;

	Material* m_material;
	std::string m_name;

public:
	float GetSizeByTime(float time) const;
	Rgba GetColorByTime(float time) const;

public:
	ParticleEmitterDefinition(const XMLElement& element);
	static ParticleType		  ToParticleType(const std::string& type);
	static LocationModuleType ToLocationModuleType(const std::string& type);
	static VelocityModuleType ToVelocityModuleType(const std::string& type);

};