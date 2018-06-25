#include "PhysicsTypes.hpp"

#ifdef TOOLS_BUILD

#include <PxPhysicsAPI.h>
#include "AABB3.hpp"
#include "Sphere3.hpp"
using namespace physx;

class PhysicsShape
{
	friend class PhysicsDynamicActor;
	friend class PhysicsStaticActor;
public:
	PhysicsShape(PxShape* pxShape);
	void SetTransform(const Matrix4& transform);
protected:
	PxShape* m_pxShape;
};

#endif