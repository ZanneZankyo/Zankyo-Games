#pragma once

#ifdef TOOLS_BUILD

#include "PhysicsTypes.hpp"
#include <PxPhysicsAPI.h>
#include "Engine\Renderer\Skeleton.hpp"
#include <extensions\PxDistanceJoint.h>
#include <vector>
using namespace physx;

class PhysicsSpringJoint
{
public:
	PhysicsSpringJoint(PhysicsDynamicActor* physicsActor, SkeletonJoint* joint, SkeletonInstance* skeletonIns);
	PhysicsSpringJoint* PairJoint(PhysicsDynamicActor* childActor, SkeletonJoint* childJoint);
	void Update(Pose& currentPose);

public:
	SkeletonInstance* m_skeletonIns = nullptr;
	SkeletonJoint* m_joint = nullptr;
	PhysicsSpringJoint* m_parent = nullptr;
	PhysicsDynamicActor* m_physicsActor = nullptr;
	std::vector<PhysicsSpringJoint*> m_children;

	
protected:
	PxSphericalJoint* m_pxJoint = nullptr;
};

#endif