#pragma once
#include <vector>
#include "Engine\Math\Matrix4.hpp"
#include "Engine\Core\EngineBase.hpp"

enum CurveEvalType
{
	LINEAR,
	SQUARED,
	CUBIC,
	QUARTIC,
	TANGENT,
};

class AnimCurveKey
{
public:
	
	float m_time;
	float m_value;
	CurveEvalType m_evalType;
	float m_inTangent;
	float m_outTangent;

	AnimCurveKey() :m_time(0), m_value(0), m_evalType(LINEAR), m_inTangent(0), m_outTangent(0) {}
	AnimCurveKey(const AnimCurveKey& copy)
		:m_time(copy.m_time), m_value(copy.m_value), m_evalType(copy.m_evalType), 
		m_inTangent(copy.m_inTangent), m_outTangent(copy.m_outTangent) {}
};

struct AnimCurveKeySet;

class AnimCurve
{
public:
	enum AnimCurveType : char
	{
		POS_X,
		POS_Y,
		POS_Z,
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		SCALE_X,
		SCALE_Y,
		SCALE_Z,
		NUM_OF_ANIM_CURVE_TYPES
	};
	enum RotationOrder : char
	{
		ROTATION_ORDER_XYZ,
		ROTATION_ORDER_XZY,
		ROTATION_ORDER_YZX,
		ROTATION_ORDER_YXZ,
		ROTATION_ORDER_ZXY,
		ROTATION_ORDER_ZYX,
		ROTATION_ORDER_SPHERIC_XYZ,
	};

	AnimCurve(const std::string& name = "");
	AnimCurve(const AnimCurve& copy);

	std::string m_name;
	RotationOrder m_rotationOrder;
	std::vector<AnimCurveKey> m_keys[NUM_OF_ANIM_CURVE_TYPES];

	Matrix4 EvaluateMatrix(float time) const;
	AnimCurveKeySet EvaluateKeySet(float time) const;
	bool IsEmpty() const;
private:
	float GetPropertyAtTime(AnimCurveType propertyType, float time, float defaultValue = 0.f) const;
};

struct AnimCurveKeySet
{
	float m_value[AnimCurve::NUM_OF_ANIM_CURVE_TYPES];
	AnimCurveKeySet()
	{ MemSetZero(m_value); }
	AnimCurveKeySet(const AnimCurveKeySet& copy)
	{
		for (char typeIndex = 0; typeIndex < AnimCurve::NUM_OF_ANIM_CURVE_TYPES; typeIndex++)
			m_value[typeIndex] = copy.m_value[typeIndex];
	}
	float& operator [] (AnimCurve::AnimCurveType type) 
	{ return m_value[type]; }
};