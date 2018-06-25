#include "Scene.hpp"
#include "Engine\Math\Physics.hpp"
#include "Engine\Math\PhysicsShape.hpp"
#include "Game.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\Skeleton.hpp"
#include "Engine\Math\PhysicsSpringJoint.hpp"

void Scene::Start()
{
	m_pxScene = PhysicsSystem::CreateScene();

	m_sphere = PhysicsSystem::CreateDynamicActor(Matrix4::CreateTranslation(Vector3(0.f,5.f,0.f)));
	m_sphere->AddShape(PhysicsSystem::CreateShapeSphere(1));
	m_pxScene->AddActor(m_sphere);

	m_plane = PhysicsSystem::CreateStaticActor();
	m_plane->AddShape(PhysicsSystem::CreateShapePlane());
	m_pxScene->AddActor(m_plane);

	auto renderer = Game::GetRenderer();
	auto vertices = MeshBuilder::Sphere(Vector3::ZERO, 10.f, 64);
	m_sphereVbo = renderer->CreateVertexBuffer(vertices, PRIMITIVE_QUADS);

	BuildUnityChanJoints();
}

void Scene::Update()
{
	m_pxScene->Update();
}

void Scene::Render() const
{
	auto renderer = Game::GetRenderer();
	renderer->PushMatrix();
	renderer->SetMaterial("Default");
	for (auto& joint : m_physicsJoints)
	{
		renderer->PushMatrix(Matrix4::CreateScale(0.01f));
		renderer->MultMatrix(joint->GetTransform());
		renderer->DrawVbo(m_sphereVbo);
		renderer->PopMatrix();
	}
	renderer->PopMatrix();
}

void Scene::BuildUnityChanJoints()
{
	/*
	//Left tail
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_01"), m_skeletonInstance->GetJoint("J_L_HairTail_02"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_02"), m_skeletonInstance->GetJoint("J_L_HairTail_03"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_03"), m_skeletonInstance->GetJoint("J_L_HairTail_04"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_04"), m_skeletonInstance->GetJoint("J_L_HairTail_05"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_05"), m_skeletonInstance->GetJoint("J_L_HairTail_06"), m_skeletonInstance));
	//Right tail
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_01"), m_skeletonInstance->GetJoint("J_R_HairTail_02"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_02"), m_skeletonInstance->GetJoint("J_R_HairTail_03"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_03"), m_skeletonInstance->GetJoint("J_R_HairTail_04"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_04"), m_skeletonInstance->GetJoint("J_R_HairTail_05"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_05"), m_skeletonInstance->GetJoint("J_R_HairTail_06"), m_skeletonInstance));
	*/
	Skeleton* skel = Game::GetInstance()->m_skeleton;
	SkeletonInstance* skelIns = Game::GetInstance()->m_skeletonInstance;

	SkeletonJoint* lHairTailJoint00 = skelIns->GetJoint("J_L_HairTail_00");
	SkeletonJoint* lHairTailJoint01 = skelIns->GetJoint("J_L_HairTail_01");
	SkeletonJoint* lHairTailJoint02 = skelIns->GetJoint("J_L_HairTail_02");
	SkeletonJoint* lHairTailJoint03 = skelIns->GetJoint("J_L_HairTail_03");
	SkeletonJoint* lHairTailJoint04 = skelIns->GetJoint("J_L_HairTail_04");
	SkeletonJoint* lHairTailJoint05 = skelIns->GetJoint("J_L_HairTail_05");
	SkeletonJoint* lHairTailJoint06 = skelIns->GetJoint("J_L_HairTail_06");

	SkeletonJoint* rHairTailJoint00 = skelIns->GetJoint("J_R_HairTail_00");
	SkeletonJoint* rHairTailJoint01 = skelIns->GetJoint("J_R_HairTail_01");
	SkeletonJoint* rHairTailJoint02 = skelIns->GetJoint("J_R_HairTail_02");
	SkeletonJoint* rHairTailJoint03 = skelIns->GetJoint("J_R_HairTail_03");
	SkeletonJoint* rHairTailJoint04 = skelIns->GetJoint("J_R_HairTail_04");
	SkeletonJoint* rHairTailJoint05 = skelIns->GetJoint("J_R_HairTail_05");
	SkeletonJoint* rHairTailJoint06 = skelIns->GetJoint("J_R_HairTail_06");

	PhysicsRigidActor* lHairTailActor00 = PhysicsSystem::CreateRigidActor(lHairTailJoint00->m_transform);
	PhysicsRigidActor* lHairTailActor01 = PhysicsSystem::CreateRigidActor(lHairTailJoint01->m_transform);
	PhysicsRigidActor* lHairTailActor02 = PhysicsSystem::CreateRigidActor(lHairTailJoint02->m_transform);
	PhysicsRigidActor* lHairTailActor03 = PhysicsSystem::CreateRigidActor(lHairTailJoint03->m_transform);
	PhysicsRigidActor* lHairTailActor04 = PhysicsSystem::CreateRigidActor(lHairTailJoint04->m_transform);
	PhysicsRigidActor* lHairTailActor05 = PhysicsSystem::CreateRigidActor(lHairTailJoint05->m_transform);
	PhysicsRigidActor* lHairTailActor06 = PhysicsSystem::CreateRigidActor(lHairTailJoint06->m_transform);

	PhysicsRigidActor* rHairTailActor00 = PhysicsSystem::CreateRigidActor(rHairTailJoint00->m_transform);
	PhysicsRigidActor* rHairTailActor01 = PhysicsSystem::CreateRigidActor(rHairTailJoint01->m_transform);
	PhysicsRigidActor* rHairTailActor02 = PhysicsSystem::CreateRigidActor(rHairTailJoint02->m_transform);
	PhysicsRigidActor* rHairTailActor03 = PhysicsSystem::CreateRigidActor(rHairTailJoint03->m_transform);
	PhysicsRigidActor* rHairTailActor04 = PhysicsSystem::CreateRigidActor(rHairTailJoint04->m_transform);
	PhysicsRigidActor* rHairTailActor05 = PhysicsSystem::CreateRigidActor(rHairTailJoint05->m_transform);
	PhysicsRigidActor* rHairTailActor06 = PhysicsSystem::CreateRigidActor(rHairTailJoint06->m_transform);

	m_physicsJoints.push_back(lHairTailActor00);
	m_physicsJoints.push_back(lHairTailActor01);
	m_physicsJoints.push_back(lHairTailActor02);
	m_physicsJoints.push_back(lHairTailActor03);
	m_physicsJoints.push_back(lHairTailActor04);
	m_physicsJoints.push_back(lHairTailActor05);
	m_physicsJoints.push_back(lHairTailActor06);

	m_physicsJoints.push_back(rHairTailActor00);
	m_physicsJoints.push_back(rHairTailActor01);
	m_physicsJoints.push_back(rHairTailActor02);
	m_physicsJoints.push_back(rHairTailActor03);
	m_physicsJoints.push_back(rHairTailActor04);
	m_physicsJoints.push_back(rHairTailActor05);
	m_physicsJoints.push_back(rHairTailActor06);

	lHairTailActor00->SetMass(10000.f);
	lHairTailActor01->SetMass(1000.f);
	lHairTailActor02->SetMass(100.f);
	lHairTailActor03->SetMass(10.f);
	lHairTailActor04->SetMass(1.f);
	lHairTailActor05->SetMass(0.1f);
	lHairTailActor06->SetMass(0.01f);

	rHairTailActor00->SetMass(10000.f);
	rHairTailActor01->SetMass(1000.f);
	rHairTailActor02->SetMass(100.f);
	rHairTailActor03->SetMass(10.f);
	rHairTailActor04->SetMass(1.f);
	rHairTailActor05->SetMass(0.1f);
	rHairTailActor06->SetMass(0.01f);

	m_pxScene->AddActor(lHairTailActor00);
	m_pxScene->AddActor(lHairTailActor01);
	m_pxScene->AddActor(lHairTailActor02);
	m_pxScene->AddActor(lHairTailActor03);
	m_pxScene->AddActor(lHairTailActor04);
	m_pxScene->AddActor(lHairTailActor05);
	m_pxScene->AddActor(lHairTailActor06);

	m_pxScene->AddActor(rHairTailActor00);
	m_pxScene->AddActor(rHairTailActor01);
	m_pxScene->AddActor(rHairTailActor02);
	m_pxScene->AddActor(rHairTailActor03);
	m_pxScene->AddActor(rHairTailActor04);
	m_pxScene->AddActor(rHairTailActor05);
	m_pxScene->AddActor(rHairTailActor06);

	m_lHairSpringJoint = new PhysicsSpringJoint(lHairTailActor00, lHairTailJoint00, skelIns);
	m_lHairSpringJoint
		->PairJoint(lHairTailActor01, lHairTailJoint01)
		->PairJoint(lHairTailActor02, lHairTailJoint02)
		->PairJoint(lHairTailActor03, lHairTailJoint03)
		->PairJoint(lHairTailActor04, lHairTailJoint04)
		->PairJoint(lHairTailActor05, lHairTailJoint05)
		->PairJoint(lHairTailActor06, lHairTailJoint06);

	m_rHairSpringJoint = new PhysicsSpringJoint(rHairTailActor00, rHairTailJoint00, skelIns);
	m_rHairSpringJoint
		->PairJoint(rHairTailActor01, rHairTailJoint01)
		->PairJoint(rHairTailActor02, rHairTailJoint02)
		->PairJoint(rHairTailActor03, rHairTailJoint03)
		->PairJoint(rHairTailActor04, rHairTailJoint04)
		->PairJoint(rHairTailActor05, rHairTailJoint05)
		->PairJoint(rHairTailActor06, rHairTailJoint06);
}
