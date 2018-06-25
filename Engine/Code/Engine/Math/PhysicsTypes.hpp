#pragma once

#ifdef TOOLS_BUILD

#include "Matrix4.hpp"
#include "Vector3.hpp"
#include <PxPhysicsAPI.h>
#include "Vector4.hpp"
using namespace physx;

class PhysicsSystem;
class PhysicsScene;
class PhysicsActor;
class PhysicsShape;
class PhysicsDynamicActor;
class PhysicsStaticActor;

struct PhysicsMaterialParameters
{
	float staticFriction = 0;
	float dynamicFriction = 0;
	float restitution = 0;
	PhysicsMaterialParameters(float sf = 0, float df = 0, float r = 0) : staticFriction(sf), dynamicFriction(df), restitution(r) {}
};

Vector3 ToVector3(const PxVec3& vec);
Vector4 ToVector4(const PxVec4& vec);
Matrix4 ToMatrix4(const PxTransform& transform);
PxVec3 ToPxVec3(const Vector3& vec);
PxVec4 ToPxVec4(const Vector4& vec);
PxTransform ToPxTransform(const Matrix4& tranform);

#endif