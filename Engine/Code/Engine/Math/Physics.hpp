#pragma once

#ifdef TOOLS_BUILD

#include "PhysicsTypes.hpp"
#include "Vector3.hpp"
#include <extensions\PxDefaultErrorCallback.h>
#include <extensions\PxDefaultAllocator.h>
#include <cudamanager\PxCudaContextManager.h>
#include "AABB2.hpp"
#include "AABB3.hpp"
#include <vector>


class PhysicsSystem
{
public:
	PhysicsSystem();

	//Scene
	friend class PhysicsSpringJoint;

public:

	static void StartUp();
	static void ShutDown();
	static PhysicsSystem* GetInstance();
	static void Update();

	static PhysicsScene* CreateScene();
	static PhysicsActor* CreateActor();
	static PhysicsDynamicActor* CreateDynamicActor(const Matrix4& transform = Matrix4::IDENTITY);
	static PhysicsStaticActor* CreateStaticActor(const Matrix4& transform = Matrix4::IDENTITY);

	

	//Shapes
	static PhysicsShape* CreateShapeBox(const Vector3& halfExtends, const PhysicsMaterialParameters& physicsMaterial = PhysicsMaterialParameters());
	static PhysicsShape* CreateShapePlane(const PhysicsMaterialParameters& physicsMaterial = PhysicsMaterialParameters());
	static PhysicsShape* CreateShapeSphere(float radius, const PhysicsMaterialParameters& physicsMaterial = PhysicsMaterialParameters());
	static PhysicsShape* CreateShapeCapsule(float radius, float halfHeight, const PhysicsMaterialParameters& physicsMaterial = PhysicsMaterialParameters());
	
	static std::vector<PhysicsDynamicActor*> SphereCast(const PhysicsScene* scene, const Vector3& position, float radius, PhysicsDynamicActor* ignoreActor = nullptr);
private:
	PxPvd* m_pxPvd = nullptr;
	PxPhysics* m_pxPhysics = nullptr;
	PxCudaContextManager* m_pxCudaContextManager = nullptr;
	static PxPhysics* GetPxPhysics();
private:
	static PxFoundation* s_pxFoundation;
	static PxDefaultErrorCallback s_pxDefaultErrorCallback;
	static PxDefaultAllocator s_pxDefaultAllocatorCallback;
	static PhysicsSystem* s_instance;
};

#endif