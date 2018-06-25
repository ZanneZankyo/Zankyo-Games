#include "Engine/Math/MathUtils.hpp"
#include <complex>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"



float CalcDistanceLine2ToPoint2(const Vector2 & lineStart, const Vector2 & lineEnd, const Vector2 & pos)
{
	Vector2 vectorStartToPoint = pos - lineStart;
	Vector2 vectorStartToEnd = lineEnd - lineStart;
	float projectLength = DotProduct(vectorStartToEnd, vectorStartToPoint) / vectorStartToEnd.CalcLength();
	float distance = sqrtf((vectorStartToPoint).CalcLengthSquared() - projectLength * projectLength);
	return distance;
}

float CalcDistanceLine2ToPoint2(const Ray2 & ray, const Vector2 & pos)
{
	return CalcDistanceLine2ToPoint2(ray.start, ray.end, pos);
}

float CalcDistanceRay2ToPoint2(const Vector2 & rayStart, const Vector2 & rayEnd, const Vector2 & pos)
{
	Vector2 vectorStartToPos = pos - rayStart;
	Vector2 vectorStartToEnd = rayEnd - rayStart;

	if (DotProduct(vectorStartToEnd, vectorStartToPos) < 0)
		return CalcDistance(pos, rayStart);

	return CalcDistanceLine2ToPoint2(rayStart, rayEnd, pos);
}

float CalcDistanceRay2ToPoint2(const Ray2 & ray, const Vector2 & pos)
{
	return CalcDistanceRay2ToPoint2(ray.start, ray.end, pos);
}

float CalcDistanceSegment2ToPoint2(const Vector2 & segmentStart, const Vector2 & segmentEnd, const Vector2 & pos)
{
	Vector2 vectorStartToPos = pos - segmentStart;
	Vector2 vectorStartToEnd = segmentEnd - segmentStart;
	Vector2 vectorEndToPos = pos - segmentEnd;
	Vector2 vectorEndToStart = segmentStart - segmentEnd;

	if (DotProduct(vectorStartToEnd, vectorStartToPos) < 0)
		return CalcDistance(pos, segmentStart);
	if (DotProduct(vectorEndToPos, vectorEndToStart) < 0)
		return CalcDistance(pos, segmentEnd);

	return CalcDistanceLine2ToPoint2(segmentStart, segmentEnd, pos);
}

float CalcDistanceSegment2ToPoint2(const Ray2& ray, const Vector2& pos)
{
	return CalcDistanceSegment2ToPoint2(ray.start, ray.end, pos);
}

float CalcDistanceAABB2ToPoint2(const AABB2 & aabb, const Vector2 & pos)
{
	Vector2 maxs = aabb.maxs;
	Vector2 mins = aabb.mins;

	if (mins.x <= pos.x && pos.x <= maxs.x && mins.y <= pos.y && pos.y <= maxs.y)
		return 0.f;
	if (pos.x < mins.x)
		return CalcDistanceSegment2ToPoint2(mins, Vector2(mins.x, maxs.y), pos);
	if (maxs.x < pos.x)
		return CalcDistanceSegment2ToPoint2(Vector2(maxs.x, mins.y), maxs, pos);
	if (pos.y < mins.y)
		return CalcDistanceSegment2ToPoint2(mins, Vector2(maxs.x, mins.y), pos);
	if (maxs.y < pos.y)
		return CalcDistanceSegment2ToPoint2(Vector2(mins.x, maxs.y), maxs, pos);
	return -1.f;
}

bool DoesSegment2OverlapDisc2(const Vector2 & rayStart, const Vector2 & rayEnd, const Vector2 & discPos, float discRadius)
{

	if (DotProduct(rayEnd - rayStart, discPos - rayStart) < 0 || DotProduct(rayStart - rayEnd, discPos - rayEnd) < 0)
		return false;
	if (CalcDistanceLine2ToPoint2(rayStart, rayEnd, discPos) < discRadius)
		return true;
	else
		return false;
}

bool DoesSegment2OverlapDisc2(const Ray2 & ray, const Disc2 & disc)
{
	return DoesSegment2OverlapDisc2(ray.start,ray.end,disc.position,disc.radius);
}

bool DoesDisc2OverlapAABB2(const Disc2 & disc, const AABB2 & aabb)
{
	return CalcDistanceAABB2ToPoint2(aabb,disc.position) < disc.radius;
}

bool DoesDisc2OverlapCapsule2(const Disc2 & disc, const Capsule2 & capsule)
{
	Vector2 center = disc.position;
	Ray2 segment = capsule.segment;
	return CalcDistanceSegment2ToPoint2(segment, center) < capsule.radius + disc.radius;
}

Vector2 CalcProjectionPoint2OnLine2(const Vector2 & start, const Vector2 & end, const Vector2 & point)
{
	Vector2 vectorStartToEnd = end - start;
	Vector2 vectorStartToPoint = point - start;
	Vector2 vectorStartToEndNormalized = vectorStartToEnd.GetNormalize();
	return start + vectorStartToEndNormalized * DotProduct(vectorStartToEndNormalized, vectorStartToPoint);
}

Vector2 CalcProjectionPoint2OnLine2(const Ray2 & line, const Vector2 & point)
{
	return CalcProjectionPoint2OnLine2(line.start, line.end, point);
}

Vector2 FindClosestPoint2ToLineSegment2(const Vector2 & segmentStart, const Vector2 & segmentEnd, const Vector2 & pos)
{
	Vector2 vectorStartToPos = pos - segmentStart;
	Vector2 vectorStartToEnd = segmentEnd - segmentStart;
	Vector2 vectorEndToPos = pos - segmentEnd;
	Vector2 vectorEndToStart = segmentStart - segmentEnd;

	if (DotProduct(vectorStartToEnd, vectorStartToPos) < 0)
		return segmentStart;
	if (DotProduct(vectorEndToPos, vectorEndToStart) < 0)
		return segmentEnd;

	return CalcProjectionPoint2OnLine2(segmentStart, segmentEnd, pos);
}

Vector2 FindClosestPoint2ToLineSegment2(const Ray2 & segment, const Vector2 & point)
{
	return FindClosestPoint2ToLineSegment2(segment.start, segment.end,point);
}

bool DoesZACylinderOverlapSphere3(const ZACylinder & cylinder, const Sphere3 & sphere)
{
	if (cylinder.IsPointInside(sphere.position))
		return true;

	Vector3 cylinderTop = cylinder.GetTopPosition();
	Vector3 cylinderBottom = cylinder.GetBottomPosition();

	if (cylinderTop.z + sphere.radius < sphere.position.z)
		return false;
	if (cylinderBottom.z - sphere.radius > sphere.position.z)
		return false;


	Vector2 cylinderPosProjectionInXYPlane(cylinder.centerPosition.x, cylinder.centerPosition.y);
	Vector2 spherePosProjectionInXYPlane(sphere.position.x, sphere.position.y);

	Disc2 cylinderProjectionInXYPlane(cylinderPosProjectionInXYPlane, cylinder.radius);
	Disc2 sphereProjectionInXYPlane(spherePosProjectionInXYPlane, sphere.radius);

	if (cylinderBottom.z <= sphere.position.z && sphere.position.z <= cylinderTop.z)
	{
		return DoDisc2sOverlap(sphereProjectionInXYPlane, cylinderProjectionInXYPlane);
	}

	if (!DoDisc2sOverlap(cylinderProjectionInXYPlane, sphereProjectionInXYPlane))
		return false;

	if (cylinderProjectionInXYPlane.IsPointInside(spherePosProjectionInXYPlane))
	{
		if (cylinderBottom.z - sphere.radius <= sphere.position.z && sphere.position.z <= cylinderTop.z + sphere.radius)
			return true;
	}

	{
		Vector3 delta;
		Vector3 cylinderCircleCenterPosition;
		if (cylinderTop.z < sphere.position.z)
		{
			delta = sphere.position - cylinderTop;
			cylinderCircleCenterPosition = cylinderTop;
		}
		else if(cylinderBottom.z > sphere.position.z)
		{
			delta = sphere.position - cylinderBottom;
			cylinderCircleCenterPosition = cylinderBottom;
		}
		Vector2 deltaProjectionInXYPlane(delta.x, delta.y);
		Vector2 edgePointDelta = deltaProjectionInXYPlane;
		edgePointDelta.SetLength(cylinder.radius);
		Vector3 edgePoint = cylinderCircleCenterPosition + Vector3(edgePointDelta.x, edgePointDelta.y, 0.f);
		return sphere.IsPointInside(edgePoint);
	}
}

bool DoesZACylinderOverlapAABB3(const ZACylinder & cylinder, const AABB3 & bound)
{
	Vector3 cylinderTop = cylinder.GetTopPosition();
	Vector3 cylinderBottom = cylinder.GetBottomPosition();

	if (cylinderTop.z < bound.mins.z)
		return false;
	if (cylinderBottom.z > bound.maxs.z)
		return false;

	AABB2 boundProjectionInXYPlane(Vector2(bound.mins), Vector2(bound.maxs));
	Disc2 cylinderProjectionInXYPlane(Vector2(cylinder.centerPosition), cylinder.radius);
	return DoesDisc2OverlapAABB2(cylinderProjectionInXYPlane, boundProjectionInXYPlane);
}

float CalcShortestAngularDistance(float startDegrees, float endDegrees)
{
	float angularDistance = endDegrees - startDegrees;
	angularDistance = fmod(angularDistance, 360.f);
	while (angularDistance > 180.f)
		angularDistance -= 360.f;
	while (angularDistance < -180.f)
		angularDistance += 360.f;
	return angularDistance;
}

float CalcLongestAngularDistance(float startDegrees, float endDegrees)
{
	float angularDistance = endDegrees - startDegrees;
	angularDistance = fmod(angularDistance, 360.f);
	if (0.f < angularDistance && angularDistance < 180.f)
		angularDistance -= 360.f;
	if (-180.f < angularDistance && angularDistance < 0.f)
		angularDistance += 360.f;
	return angularDistance;
}

float TurnToward(float & currentDegrees, float goalDegrees, float maxTurnDegrees)
{
	float expectedTurningDegrees = CalcShortestAngularDistance(currentDegrees, goalDegrees);
	if (fabs(expectedTurningDegrees) < maxTurnDegrees)
	{
		currentDegrees = goalDegrees;
		return fabs(expectedTurningDegrees);
	}
	else if (expectedTurningDegrees > 0)
		currentDegrees += maxTurnDegrees;
	else
		currentDegrees -= maxTurnDegrees;

	return fabs(maxTurnDegrees);
}

float AreAlmostEqual(float a, float b, float tolerate)
{
	return fabs(a-b) < tolerate;
}

float Ceiling(float number)
{
	int intNum = (int)number;
	if (number == intNum)
		return number;
	if (number > 0.f)
		return intNum + 1.f;
	return (float) intNum;
}

