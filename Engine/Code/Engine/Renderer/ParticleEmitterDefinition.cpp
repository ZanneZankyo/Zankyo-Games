#include "ParticleEmitterDefinition.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\StringUtils.hpp"

float ParticleEmitterDefinition::GetSizeByTime(float time) const
{
	return m_sizeByLife.AtTime(1.f - time / m_lifeTime);
}

Rgba ParticleEmitterDefinition::GetColorByTime(float time) const
{
	return m_colorByLife.AtTime(1.f - time / m_lifeTime);
}

ParticleEmitterDefinition::ParticleEmitterDefinition(const XMLElement& element)
{
	m_name = element.GetAttribute("name");
	m_particleType = ToParticleType(element.GetAttribute("type"));
	XMLElement locationElement = element.GetChildNode("Location");
	m_locationModuleType = ToLocationModuleType(locationElement.GetAttribute("module"));
	switch (m_locationModuleType)
	{
	case ParticleEmitterDefinition::LOCATION_BOX:
		m_locationModule = new ParticleEmitterLocationModuleBox(locationElement);
		break;
	case ParticleEmitterDefinition::LOCATION_SPHERE:
		m_locationModule = new ParticleEmitterLocationModuleSphere(locationElement);
		break;
	default:
		break;
	}
	XMLElement velocityElement = element.GetChildNode("Velocity");
	m_velocityModuleType = ToVelocityModuleType(velocityElement.GetAttribute("module"));
	switch (m_velocityModuleType)
	{
	case ParticleEmitterDefinition::VELOCITY_BOX:
		m_velocityModule = new ParticleEmitterVelocityModuleBox(velocityElement);
		break;
	case ParticleEmitterDefinition::VELOCITY_LINE:
		m_velocityModule = new ParticleEmitterVelocityModuleLine(velocityElement);
		break;
	case ParticleEmitterDefinition::VELOCITY_SPHERE:
		m_velocityModule = new ParticleEmitterVelocityModuleSphere(velocityElement);
		break;
	case ParticleEmitterDefinition::VELOCITY_CONE:
		m_velocityModule = new ParticleEmitterVelocityModuleCone(velocityElement);
		break;
	default:
		break;
	}
	m_lifeTime = stof(element.GetChildNode("LifeTime").GetText());
	m_spawnTime = stof(element.GetChildNode("SpawnTime").GetText());
	m_burst = stoi(element.GetChildNode("Burst").GetText());
	m_spawn = stoi(element.GetChildNode("Spawn").GetText());

	XMLElement colorElem = element.GetChildNode("ColorByLife");
	for (auto& child : colorElem.GetChildNodes("KeyFrame"))
	{
		std::string fractionStr = child.GetAttribute("fraction");
		std::string colorStr = child.GetAttribute("value");
		float fraction = stof(fractionStr);
		Rgba color(colorStr);
		m_colorByLife.AddKey(fraction, color);
	}
	/*std::string startColorStr = colorElem.GetChildNode("Start").GetText();
	std::string endColorStr = colorElem.GetChildNode("End").GetText();
	m_startColor = Rgba(startColorStr);
	m_endColor = Rgba(endColorStr);*/

	XMLElement sizeElem = element.GetChildNode("SizeByLife");
	for (auto& child : sizeElem.GetChildNodes("KeyFrame"))
	{
		std::string fractionStr = child.GetAttribute("fraction");
		std::string sizeStr = child.GetAttribute("value");
		float fraction = stof(fractionStr);
		float size = stof(sizeStr);
		m_sizeByLife.AddKey(fraction, size);
	}
	//m_startSize = stof(sizeElem.GetChildNode("Start").GetText());
	//m_endSize = stof(sizeElem.GetChildNode("End").GetText());

	m_constantAcceleration = StringUtils::ToVector3(element.GetChildNode("ConstantAcceleration").GetText());
	
	m_doesPrewarm = StringUtils::ToBool(element.GetChildNode("Prewarm").GetText());

	m_material = Material::CreateOrGetMaterial(element.GetChildNode("Material").GetText());
}

ParticleEmitterDefinition::ParticleType ParticleEmitterDefinition::ToParticleType(const std::string& type)
{
	if (type == "ribbon")
		return RIBBON;
	else if (type == "mesh")
		return MESH;
	return DEFAULT;
}

ParticleEmitterDefinition::LocationModuleType ParticleEmitterDefinition::ToLocationModuleType(const std::string& type)
{
	if (type == "box")
		return LOCATION_BOX;
	else if (type == "sphere")
		return LOCATION_SPHERE;
	ASSERT_OR_DIE(false, "ParticleEmitterDefinition::ToLocationModuleType:\nFailed to convert to LocationModuleType!\n");
	return LOCATION_BOX;
}

ParticleEmitterDefinition::VelocityModuleType ParticleEmitterDefinition::ToVelocityModuleType(const std::string& type)
{
	if (type == "cone")
		return VELOCITY_CONE;
	else if (type == "box")
		return VELOCITY_BOX;
	else if (type == "line")
		return VELOCITY_LINE;
	else if (type == "sphere")
		return VELOCITY_SPHERE;
	ASSERT_OR_DIE(false, "ParticleEmitterDefinition::ToVelocityModuleType:\nFailed to convert to VelocityModuleType!\n");
	return VELOCITY_LINE;
}

ParticleEmitterLocationModuleBox::ParticleEmitterLocationModuleBox(const XMLElement& element)
	: ParticleEmitterLocationModuleBox()
{
	XMLElement minsElem = element.GetChildNode("Mins");
	Vector3 mins = StringUtils::ToVector3(minsElem.GetText());
	XMLElement maxsElem = element.GetChildNode("Maxs");
	Vector3 maxs = StringUtils::ToVector3(minsElem.GetText());
	m_box.mins = mins;
	m_box.maxs = maxs;
}

ParticleEmitterLocationModuleSphere::ParticleEmitterLocationModuleSphere(const XMLElement& element)
	: ParticleEmitterLocationModuleSphere()
{
	XMLElement centerElem = element.GetChildNode("Center");
	Vector3 center = StringUtils::ToVector3(centerElem.GetText());
	XMLElement minRadiusElem = element.GetChildNode("MinRadius");
	float minRadius = stof(minRadiusElem.GetText());
	XMLElement maxRadiusElem = element.GetChildNode("MaxRadius");
	float maxRadius = stof(maxRadiusElem.GetText());
	m_minSphere.position = center;
	m_minSphere.radius = minRadius;
	m_maxSphere.position = center;
	m_maxSphere.radius = maxRadius;
}

ParticleEmitterVelocityModuleBox::ParticleEmitterVelocityModuleBox(const XMLElement& element)
	: ParticleEmitterVelocityModuleBox()
{
	XMLElement minsElem = element.GetChildNode("Mins");
	Vector3 mins = StringUtils::ToVector3(minsElem.GetText());
	XMLElement maxsElem = element.GetChildNode("Maxs");
	Vector3 maxs = StringUtils::ToVector3(minsElem.GetText());
	m_box.mins = mins;
	m_box.maxs = maxs;
}

ParticleEmitterVelocityModuleLine::ParticleEmitterVelocityModuleLine(const XMLElement& element)
	: ParticleEmitterVelocityModuleLine()
{
	XMLElement minVelElem = element.GetChildNode("MinVelocity");
	XMLElement maxVelElem = element.GetChildNode("MaxVelocity");
	m_minVelocity = StringUtils::ToVector3(minVelElem.GetText());
	m_maxVelocity = StringUtils::ToVector3(maxVelElem.GetText());
}

ParticleEmitterVelocityModuleCone::ParticleEmitterVelocityModuleCone(const XMLElement& element)
	: ParticleEmitterVelocityModuleCone()
{
	XMLElement dirElem = element.GetChildNode("Direction");
	XMLElement minSpdElem = element.GetChildNode("MinSpeed");
	XMLElement maxSpdElem = element.GetChildNode("MaxSpeed");
	XMLElement angleElem = element.GetChildNode("Angle");
	m_direction = StringUtils::ToVector3(dirElem.GetText());
	m_minSpeed = stof(minSpdElem.GetText());
	m_maxSpeed = stof(maxSpdElem.GetText());
	m_angle = stof(angleElem.GetText());
}

ParticleEmitterVelocityModuleSphere::ParticleEmitterVelocityModuleSphere(const XMLElement& element)
	: ParticleEmitterVelocityModuleSphere()
{
	XMLElement centerElem = element.GetChildNode("Center");
	Vector3 center = StringUtils::ToVector3(centerElem.GetText());
	XMLElement minRadiusElem = element.GetChildNode("MinRadius");
	float minRadius = stof(minRadiusElem.GetText());
	XMLElement maxRadiusElem = element.GetChildNode("MaxRadius");
	float maxRadius = stof(maxRadiusElem.GetText());
	m_minSphere.position = center;
	m_minSphere.radius = minRadius;
	m_maxSphere.position = center;
	m_maxSphere.radius = maxRadius;
}
