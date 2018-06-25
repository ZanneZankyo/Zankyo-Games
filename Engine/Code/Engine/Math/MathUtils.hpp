#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Ray2.hpp"
#include "Capsule2.hpp"
#include "ZACylinder3.hpp"
#include "Sphere3.hpp"
#include "AABB3.hpp"

#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b
constexpr float M_PI = 3.1415926535897932384626433832795f;

float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);

int GetRandomIntLessThan(int maxValueNotInclusive);
int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive);
int GetRandomIntFromArray(const int* integersToPick, int numberOfIntegers);
int GetPlusOneOrMinusOne();

float GetRandomFloatZeroToOne();
float GetRandomFloatMinusOneToPlusOne();
float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive);
bool IsPercentChance(float probability);

float SinDegrees(float degrees);
float CosDegrees(float degrees);
float Atan2Degrees(float y,float x);

float CalcDistanceLine2ToPoint2(const Vector2& lineStart, const Vector2& lineEnd, const Vector2& pos);
float CalcDistanceLine2ToPoint2(const Ray2& line, const Vector2& pos);

float CalcDistanceRay2ToPoint2(const Vector2& rayStart, const Vector2& rayEnd, const Vector2& pos);
float CalcDistanceRay2ToPoint2(const Ray2& ray, const Vector2& pos);

float CalcDistanceSegment2ToPoint2(const Vector2& segmentStart, const Vector2& segmentEnd, const Vector2& pos);
float CalcDistanceSegment2ToPoint2(const Ray2& segment, const Vector2& pos);

float CalcDistanceAABB2ToPoint2(const AABB2& aabb, const Vector2& pos);

bool DoesSegment2OverlapDisc2(const Vector2& rayStart, const Vector2& rayEnd, const Vector2& discPos, float discRadius);
bool DoesSegment2OverlapDisc2(const Ray2& ray, const Disc2& disc);

bool DoesDisc2OverlapAABB2(const Disc2& disc, const AABB2& aabb);
bool DoesDisc2OverlapCapsule2(const Disc2& disc, const Capsule2& capsule);

Vector2 CalcProjectionPoint2OnLine2(const Vector2& start, const Vector2& end, const Vector2& point);
Vector2 CalcProjectionPoint2OnLine2(const Ray2& line, const Vector2& point);

Vector2 FindClosestPoint2ToLineSegment2(const Vector2& segmentStart, const Vector2& segmentEnd, const Vector2& point);
Vector2 FindClosestPoint2ToLineSegment2(const Ray2& segment, const Vector2& point);

bool DoesZACylinderOverlapSphere3(const ZACylinder& cylinder, const Sphere3& sphere);
bool DoesZACylinderOverlapAABB3(const ZACylinder& cylinder, const AABB3& bound);

float RangeMap(float in, float inMin, float inMax, float outMin, float outMax);
float Clamp(float in, float min, float max);
int Clamp(int in, int min, int max);
float SmoothStep3(float t);

float CalcShortestAngularDistance(float startDegrees, float endDegrees);
float CalcLongestAngularDistance(float startDegrees, float endDegrees);
float TurnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees);

float AreAlmostEqual(float a, float b, float tolerate);

float Ceiling(float number);

template <typename BitType>
void SetFlag(BitType& bitFlags, BitType flagBit);
template <typename BitType>
void ClearFlag(BitType& bitFlags, BitType flagBit);

template <typename Shape>
const Shape Interpolate(const Shape& start, const Shape& end, float fractionToEnd);

template <typename Type>
Type& RandomlyPickOne(Type* objects, int numOfObjects);

template<typename BitType>
inline void SetFlag(BitType & bitFlags, BitType flagBit)
{
	bitFlags |= flagBit;
}

template<typename BitType>
inline void ClearFlag(BitType & bitFlags, BitType flagBit)
{
	bitFlags &= ~flagBit;
}

//------------------------------------------------------------------------------------------------------------
template<typename Shape>
inline const Shape Interpolate(const Shape & start, const Shape & end, float fractionToEnd)
{
	float fractionToStart = 1.f - fractionToEnd;
	return start * fractionToStart + end * fractionToEnd;
}

template<typename Type>
inline Type & RandomlyPickOne(Type * objects, int numOfObjects)
{
	return objects[GetRandomIntLessThan(numOfObjects)];
}


inline float ConvertRadiansToDegrees(float radians)
{
	return radians / M_PI * 180.f;
}

inline float ConvertDegreesToRadians(float degrees)
{
	return degrees / 180.f * M_PI;
}

inline int GetRandomIntLessThan(int maxValueNotInclusive)
{
	int randomInt = rand();
	return randomInt % maxValueNotInclusive;
}

inline int GetRandomIntInRange(int minValueInclusive, int maxValueInclusive)
{
	int numOfPossibilities = maxValueInclusive - minValueInclusive + 1;
	return minValueInclusive + GetRandomIntLessThan(numOfPossibilities);
}

inline int GetRandomIntFromArray(const int * integersToPick, int numberOfIntegers)
{
	int indexToPick = GetRandomIntLessThan(numberOfIntegers);
	return integersToPick[indexToPick];
}

inline int GetPlusOneOrMinusOne()
{
	int num = GetRandomIntLessThan(2);
	if (num)
		return 1;
	return -1;
}

inline float GetRandomFloatZeroToOne()
{
	return static_cast<float> (rand()) / static_cast<float>(RAND_MAX);
	//return (float) rand() / (float) RAND_MAX;
}

inline float GetRandomFloatMinusOneToPlusOne()
{
	return GetRandomFloatInRange(-1.f, 1.f);
}

inline float GetRandomFloatInRange(float minimumInclusive, float maximumInclusive)
{
	float scale = GetRandomFloatZeroToOne();
	return minimumInclusive + scale * (maximumInclusive - minimumInclusive);
}

inline bool IsPercentChance(float probability)
{
	float roll = GetRandomFloatZeroToOne();
	return roll < probability;
}

inline float SinDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return sin(radians);
}

inline float CosDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return cos(radians);
}

inline float Atan2Degrees(float y, float x)
{
	float radians = atan2(y, x);
	return ConvertRadiansToDegrees(radians);
}

inline float RangeMap(float in, float inMin, float inMax, float outMin, float outMax)
{
	if (in < inMin)
		in = inMin;
	if (in > inMax)
		in = inMax;
	return (in - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
}

inline float Clamp(float in, float min, float max)
{
	if (in < min)
		in = min;
	if (in > max)
		in = max;
	return in;
}

inline int Clamp(int in, int min, int max)
{
	if (in < min)
		in = min;
	if (in > max)
		in = max;
	return in;
}

inline float SmoothStep3(float t)
{
	return t * t * (3.f - 2.f * t);
}