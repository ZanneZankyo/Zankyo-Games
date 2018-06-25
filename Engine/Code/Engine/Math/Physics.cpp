
#ifdef TOOLS_BUILD

#include "Physics.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "PhysicsScene.hpp"
#include "PhysicsShape.hpp"
#include "PhysicsDynamicActor.hpp"
#include "PhysicsStaticActor.hpp"


PxFoundation* PhysicsSystem::s_pxFoundation = nullptr;
PxDefaultErrorCallback PhysicsSystem::s_pxDefaultErrorCallback;
PxDefaultAllocator PhysicsSystem::s_pxDefaultAllocatorCallback;
PhysicsSystem* PhysicsSystem::s_instance = nullptr;

PhysicsSystem::PhysicsSystem()
{
	
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5435, 10000);
	//m_pxPvd = PxCreatePvd(*s_pxFoundation);
	//m_pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	PxTolerancesScale pxScale;
	pxScale.length = 100;
	pxScale.mass = 1000;
	pxScale.speed = 980;
	m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *s_pxFoundation,
		pxScale,true);
	ASSERT_OR_DIE(m_pxPhysics, "PhysicsSystem: PxCreatePhysics() failed!");

	PxCudaContextManagerDesc cudaContextManagerDesc;
	m_pxCudaContextManager = PxCreateCudaContextManager(*s_pxFoundation, cudaContextManagerDesc);
	if (m_pxCudaContextManager)
	{
		if (!m_pxCudaContextManager->contextIsValid())
		{
			m_pxCudaContextManager->release();
			m_pxCudaContextManager = NULL;
		}
	}
}



PhysicsScene* PhysicsSystem::CreateScene()
{
	PxTolerancesScale scale;
	scale.length = 100;
	scale.mass = 1000;
	scale.speed = 980;
	PxSceneDesc desc(scale);
	desc.gravity = PxVec3(0.f, -9.8f, 0.f);
	desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	desc.filterShader = PxDefaultSimulationFilterShader;

/*
#if PX_SUPPORT_GPU_PHYSX
	if (!desc.gpuDispatcher && s_instance->m_pxCudaContextManager)
		desc.gpuDispatcher = s_instance->m_pxCudaContextManager->getGpuDispatcher();
#endif
*/
	//desc.frictionType = PxFrictionType::eTWO_DIRECTIONAL;
	//desc.frictionType = PxFrictionType::eONE_DIRECTIONAL;
	desc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	desc.flags |= PxSceneFlag::eENABLE_PCM;
	//desc.flags |= PxSceneFlag::eENABLE_AVERAGE_POINT;
	desc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	//desc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
	desc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	desc.flags |= PxSceneFlag::eSUPPRESS_EAGER_SCENE_QUERY_REFIT;
	//desc.flags |= PxSceneFlag::eDISABLE_CONTACT_CACHE;
	desc.broadPhaseType = PxBroadPhaseType::eGPU;
	desc.gpuMaxNumPartitions = 8;
	//PxPhysics::createScene(desc)
	PxScene* pxScene = GetPxPhysics()->createScene(desc);
	return new PhysicsScene(pxScene);
}

PhysicsDynamicActor* PhysicsSystem::CreateDynamicActor(const Matrix4& transform)
{
	PxTransform pxTransform = ToPxTransform(transform);
	PxRigidDynamic* pxRigidActor = GetPxPhysics()->createRigidDynamic(pxTransform);
	return new PhysicsDynamicActor(pxRigidActor);
}

PhysicsStaticActor* PhysicsSystem::CreateStaticActor(const Matrix4& transform /*= Matrix4::IDENTITY*/)
{
	PxTransform pxTransform = ToPxTransform(transform);
	PxRigidStatic* pxStaticActor = GetPxPhysics()->createRigidStatic(pxTransform);
	return new PhysicsStaticActor(pxStaticActor);
}

PhysicsShape* PhysicsSystem::CreateShapeBox(const Vector3& halfExtends, const PhysicsMaterialParameters& physicsMaterial /*= PhysicsMaterialParameters()*/)
{
	PxBoxGeometry* pxBox = new PxBoxGeometry(halfExtends.x, halfExtends.y, halfExtends.z);
	PxMaterial* pxMaterial = GetPxPhysics()->createMaterial(physicsMaterial.staticFriction, physicsMaterial.dynamicFriction, physicsMaterial.restitution);
	PxShape* pxShape = GetPxPhysics()->createShape(*pxBox, *pxMaterial);
	return new PhysicsShape(pxShape);
}

PhysicsShape* PhysicsSystem::CreateShapePlane(const PhysicsMaterialParameters& physicsMaterial /*= PhysicsMaterialParameters()*/)
{
	PxPlaneGeometry* pxPlane = new PxPlaneGeometry();
	PxMaterial* pxMaterial = GetPxPhysics()->createMaterial(physicsMaterial.staticFriction, physicsMaterial.dynamicFriction, physicsMaterial.restitution);
	PxShape* pxShape = GetPxPhysics()->createShape(*pxPlane, *pxMaterial);
	return new PhysicsShape(pxShape);
}

PhysicsShape* PhysicsSystem::CreateShapeSphere(float radius, const PhysicsMaterialParameters& physicsMaterial /*= PhysicsMaterialParameters()*/)
{
	//PxGeometryType type = PxGeometryType::Enum::eSPHERE
	//PxGeometry* pxGeometry = new PxGeometry(PxGeometryType::Enum::eSPHERE);
	PxSphereGeometry* pxSphere = new PxSphereGeometry(radius);
	PxMaterial* pxMaterial = GetPxPhysics()->createMaterial(physicsMaterial.staticFriction, physicsMaterial.dynamicFriction, physicsMaterial.restitution);
	PxShape* pxShape = GetPxPhysics()->createShape(*pxSphere, *pxMaterial);
	return new PhysicsShape(pxShape);
}

PhysicsShape* PhysicsSystem::CreateShapeCapsule(float radius, float halfHeight, const PhysicsMaterialParameters& physicsMaterial /*= PhysicsMaterialParameters()*/)
{
	PxCapsuleGeometry* pxSphere = new PxCapsuleGeometry(radius, halfHeight);
	PxMaterial* pxMaterial = GetPxPhysics()->createMaterial(physicsMaterial.staticFriction, physicsMaterial.dynamicFriction, physicsMaterial.restitution);
	PxShape* pxShape = GetPxPhysics()->createShape(*pxSphere, *pxMaterial);
	return new PhysicsShape(pxShape);
}

std::vector<PhysicsDynamicActor*> PhysicsSystem::SphereCast(const PhysicsScene* scene, const Vector3& position, float radius, PhysicsDynamicActor* ignoreActor /*= nullptr*/)
{
	std::vector<PhysicsDynamicActor*> results;

	PxOverlapBuffer queryHits;														// [out] Overlap results
	PxGeometry overlapShape = PxSphereGeometry(radius);							// [in] shape to test for overlaps
	PxTransform shapePose = ToPxTransform(Matrix4::CreateTranslation(position));// [in] initial shape pose (at distance=0)

	bool status = scene->m_pxScene->overlap(overlapShape, shapePose, queryHits);
	if (!status)
		return results;
	int numOfHits = queryHits.getNbAnyHits();
	for (int hitIndex = 0; hitIndex < numOfHits; hitIndex++)
	{
		const PxOverlapHit& hit = queryHits.getAnyHit(hitIndex);
		if (hit.actor != ignoreActor->m_pxRigidActor)
			results.push_back((PhysicsDynamicActor*)hit.actor->userData);
	}
}

void PhysicsSystem::StartUp()
{
	s_pxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, s_pxDefaultAllocatorCallback,
		s_pxDefaultErrorCallback);
	ASSERT_OR_DIE(s_pxFoundation, "PhysicsSystem: PxCreateFoundation() failed!");
	s_instance = new PhysicsSystem();
}

void PhysicsSystem::ShutDown()
{
	GetPxPhysics()->release();
	s_pxFoundation->release();
}

PhysicsSystem* PhysicsSystem::GetInstance()
{
	return s_instance;
}


void PhysicsSystem::Update()
{

}

physx::PxPhysics* PhysicsSystem::GetPxPhysics()
{
	return s_instance->m_pxPhysics;
}

#endif