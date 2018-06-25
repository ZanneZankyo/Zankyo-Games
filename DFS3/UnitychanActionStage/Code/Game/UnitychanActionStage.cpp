#include "UnitychanActionStage.hpp"
#include "Game.hpp"
#include "Engine\Math\Physics.hpp"

void UnitychanActionStage::Start()
{
	PhysicsSystem::StartUp();
	ParticleSystem::Start();
	UnitychanDefinition::Start();
	m_pxScene = PhysicsSystem::CreateScene();
	SetupFloor();

	Unitychan* unitychan = new Unitychan();
	AddEntity(unitychan);
	m_player = unitychan;
	m_controller.m_target = unitychan;
	unitychan->m_controller = &m_controller;

	Unitychan* unitychan2 = new Unitychan();
	unitychan2->SetPosition(Vector3(0, 0, 500));
	AddEntity(unitychan2);

	BuildVbo();

	m_controller.m_camera.m_position.y = 120.f;
	m_controller.m_camera.m_position.z = 400.f;
	m_controller.m_camera.m_rotation.x = 10.f;
	m_controller.m_camera.m_rotation.y = 180.f;
	m_controller.m_camera.m_far = 5000.f;
	m_controller.m_camera.m_near = 10.f;
}

void UnitychanActionStage::Update()
{
	m_controller.Update();
	for (Entity* entity : m_entities)
		entity->Update();
	m_light.SetPosition(m_player->GetSocketPosition("Character1_Hips"));
	//ParticleSystem::Update();
	if (m_pxScene)
		m_pxScene->Update();
}

void UnitychanActionStage::Render() const
{
	auto& renderer = Game::GetInstance()->m_renderer;

	renderer.SetCamera(m_controller.m_camera);
	//renderer.EnablePointLight(1, m_light);

	for (Entity* entity : m_entities)
		entity->Render();

	renderer.SetMaterial("Default");
	renderer.DrawVbo(PRIMITIVE_LINES, m_gridVbo);

	//renderer.DrawParticleSystem(m_controller.m_camera.m_position);
}

void UnitychanActionStage::AddEntity(Entity* entity)
{
	m_entities.push_back(entity);
	if (entity->m_pxActor)
		m_pxScene->AddActor(entity->m_pxActor);
}

void UnitychanActionStage::BuildVbo()
{
	std::vector<Vertex3> grids;
	Rgba gridLineColor = Rgba(180, 180, 180);

	gridLineColor.ScaleRGB(0.5f);
	float gridLength = 2000.f;
	float gridGap = 50.f;
	grids.reserve((gridLength * 2.f / gridGap) * (gridLength * 2.f / gridGap) * 4.f);
	for (float i = -gridLength; i <= gridLength; i += gridGap)
	{
		for (float j = -gridLength; j <= gridLength; j += gridGap)
		{
			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j + gridGap, 0.f, i), gridLineColor));

			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j, 0.f, i + gridGap), gridLineColor));
		}

	}
	m_gridVbo = Game::GetInstance()->m_renderer.CreateVertexBuffer(grids.data(), grids.size(), PRIMITIVE_LINES);
}

UnitychanActionStage* UnitychanActionStage::GetInstance()
{
	return Game::GetInstance()->m_actionStage;
}

void UnitychanActionStage::SetupFloor()
{
	float floorSize = 100000;
	float floorThickness = 100;
	m_floor = PhysicsSystem::CreateStaticActor(Matrix4::CreateTranslation(Vector3(0.f, -floorThickness, 0.f)));
	PhysicsShape* floorShape = PhysicsSystem::CreateShapeBox(Vector3(floorSize, floorThickness, floorSize),PhysicsMaterialParameters(0.5f,0.5f,0.f));
	m_floor->AddShape(floorShape);
	m_pxScene->AddActor(m_floor);
}
