#include "AnimCurve.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"

AnimCurve::AnimCurve(const std::string& name/* = ""*/)
	: m_name(name)
	, m_keys()
	, m_rotationOrder()
{}

AnimCurve::AnimCurve(const AnimCurve& copy)
	: m_name(copy.m_name)
	, m_rotationOrder(copy.m_rotationOrder)
{
	for (char index = 0; index < NUM_OF_ANIM_CURVE_TYPES; index++)
		m_keys[index] = copy.m_keys[index];
}

Matrix4 AnimCurve::EvaluateMatrix(float time) const
{
	float posX = GetPropertyAtTime(POS_X, time);
	float posY = GetPropertyAtTime(POS_Y, time);
	float posZ = GetPropertyAtTime(POS_Z, time);
	float rotationX = GetPropertyAtTime(ROTATION_X, time);
	float rotationY = GetPropertyAtTime(ROTATION_Y, time);
	float rotationZ = GetPropertyAtTime(ROTATION_Z, time);
	float scaleX = GetPropertyAtTime(SCALE_X, time, 1.f);
	float scaleY = GetPropertyAtTime(SCALE_Y, time, 1.f);
	float scaleZ = GetPropertyAtTime(SCALE_Z, time, 1.f);
	//Matrix4 mat = Matrix4::CreateTransform(Vector3(posX, posY, posZ), Vector3(roll, pitch, yaw), Vector3(scaleX, scaleY, scaleZ));
	Matrix4 mat = Matrix4::IDENTITY;

	Matrix4 scaleMat = Matrix4::CreateScale(Vector3(scaleX, scaleY, scaleZ));
	Matrix4 flipX = Matrix4::CreateScale(Vector3(-1.f, 1.f, 1.f));
	Matrix4 rX = Matrix4::CreateRotationDegreesAboutX(rotationX);
	Matrix4 rY = Matrix4::CreateRotationDegreesAboutY(rotationY);
	Matrix4 rZ = Matrix4::CreateRotationDegreesAboutZ(rotationZ);
	Matrix4 translateMat = Matrix4::CreateTranslation(Vector3(posX, posY, posZ));

	//mat.Scale(Vector3(scaleX, scaleY, scaleZ));
	//mat = mat * scaleMat;
	mat = mat * translateMat;
	
	switch (m_rotationOrder)
	{
	case AnimCurve::ROTATION_ORDER_XYZ:
		//mat.RotateDegreesAboutX(rotationX);
		//mat.RotateDegreesAboutY(rotationY);
		//mat.RotateDegreesAboutZ(rotationZ);
		mat = mat * rZ * rY * rX;
		break;
	case AnimCurve::ROTATION_ORDER_XZY:
		//mat.RotateDegreesAboutX(rotationX);
		//mat.RotateDegreesAboutZ(rotationZ);
		//mat.RotateDegreesAboutY(rotationY);
		//mat = mat * rX * rZ * rY;
		mat = mat * rY * rZ * rX;
		break;
	case AnimCurve::ROTATION_ORDER_YZX:
		//mat.RotateDegreesAboutY(rotationY);
		//mat.RotateDegreesAboutZ(rotationZ);
		//mat.RotateDegreesAboutX(rotationX);
		mat = mat * rX * rZ * rY;
		break;
	case AnimCurve::ROTATION_ORDER_YXZ:
		//mat.RotateDegreesAboutY(rotationY);
		//mat.RotateDegreesAboutX(rotationX);
		//mat.RotateDegreesAboutZ(rotationZ);
		mat = mat * rZ * rX * rY;
		break;
	case AnimCurve::ROTATION_ORDER_ZXY:
		//mat.RotateDegreesAboutZ(rotationZ);
		//mat.RotateDegreesAboutX(rotationX);
		//mat.RotateDegreesAboutY(rotationY);
		mat = mat * rY * rX * rZ;
		break;
	case AnimCurve::ROTATION_ORDER_ZYX:
		//mat.RotateDegreesAboutZ(rotationZ);
		//mat.RotateDegreesAboutY(rotationY);
		//mat.RotateDegreesAboutX(rotationX);
		mat = mat * rX * rY * rZ;
		break;
	case AnimCurve::ROTATION_ORDER_SPHERIC_XYZ:
		ASSERT_RECOVERABLE(false, "rotationOrder is SphericXYZ");
		break;
	default:
		ASSERT_RECOVERABLE(false, "rotationOrder is null");
		break;
	}
	
	//mat.Translate(Vector3(posX, posY, posZ));
	//mat = mat * translateMat;
	mat = mat * scaleMat;

	Vector3 translate = mat.GetPosition();
	
	//mat = flipX * mat * flipX;

	return mat;
}

AnimCurveKeySet AnimCurve::EvaluateKeySet(float time) const
{
	AnimCurveKeySet keyset;
	keyset[POS_X] = GetPropertyAtTime(POS_X, time);
	keyset[POS_Y] = GetPropertyAtTime(POS_Y, time);
	keyset[POS_Z] = GetPropertyAtTime(POS_Z, time);
	keyset[ROTATION_X] = GetPropertyAtTime(ROTATION_X, time);
	keyset[ROTATION_Y] = GetPropertyAtTime(ROTATION_Y, time);
	keyset[ROTATION_Z] = GetPropertyAtTime(ROTATION_Z, time);
	keyset[SCALE_X] = GetPropertyAtTime(SCALE_X, time, 1.f);
	keyset[SCALE_Y] = GetPropertyAtTime(SCALE_Y, time, 1.f);
	keyset[SCALE_Z] = GetPropertyAtTime(SCALE_Z, time, 1.f);
	return keyset;
}

bool AnimCurve::IsEmpty() const
{
	for (char propertyIndex = 0; propertyIndex < NUM_OF_ANIM_CURVE_TYPES; propertyIndex++)
		if (!m_keys[propertyIndex].empty())
			return false;
	return true;
}

float AnimCurve::GetPropertyAtTime(AnimCurveType propertyType, float time, float defaultValue) const
{
	const std::vector<AnimCurveKey>& keys = m_keys[propertyType];
	int frontIndex = -1;
	int endIndex = -1;
	for (size_t keyIndex = 0; keyIndex < keys.size(); keyIndex++)
	{
		if (keys[keyIndex].m_time == time)
			return keys[keyIndex].m_value;
		if (keys[keyIndex].m_time < time)
			frontIndex = keyIndex;
		else
			endIndex = keyIndex;
		if (frontIndex != -1 && endIndex != -1)
		{
			float fraction = RangeMap(time, keys[frontIndex].m_time, keys[endIndex].m_time, 0.0, 1.0);
			return Interpolate(keys[frontIndex].m_value, keys[endIndex].m_value, fraction);
		}
	}
	return defaultValue;
}
