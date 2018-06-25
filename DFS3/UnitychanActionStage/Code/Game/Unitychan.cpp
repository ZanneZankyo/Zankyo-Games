#include "Unitychan.hpp"
#include "Game.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Core\Event.hpp"
#include "Engine\Renderer\ParticleEmitter.hpp"
#include "Engine\Renderer\ParticleEffect.hpp"
#include "Engine\Renderer\ParticleSystem.hpp"
#include "Engine\Renderer\ParticleEffectDefinition.hpp"
#include "Engine\Math\Physics.hpp"
#include "Engine\Math\PhysicsShape.hpp"
#include "Engine\Math\PhysicsTypes.hpp"
#include "HitBox.hpp"
#include "Engine\Audio\Audio.hpp"

UnitychanDefinition* UnitychanDefinition::s_instance = new UnitychanDefinition();

UnitychanDefinition::~UnitychanDefinition()
{
	SAFE_DELETE(m_skeleton);
	for(Mesh* mesh : *m_modelMeshSet)
		SAFE_DELETE(mesh);
}

void UnitychanDefinition::Start()
{
	s_instance->m_modelMeshSet = Mesh::GetOrLoadSet("Data/bin/unitychan.mesh");
	if (!s_instance->m_skeleton)
		s_instance->m_skeleton = Skeleton::GetOrLoad(s_instance->m_skeletonFilePath);

	s_instance->SetUnityChanStateMachine();
	s_instance->SetUnityChanMaterial();
}

void UnitychanDefinition::SetUnityChanStateMachine()
{
	m_stateMachine.AddState("Idle", Motion::GetOrLoad("Data/bin/Idle.anim"), 0.3f);

	m_stateMachine.AddState("Run", Motion::GetOrLoad("Data/bin/Run.anim"), 0.3f);

	m_stateMachine.AddState("LeftPunch", Motion::GetOrLoad("Data/bin/LeftPunch.anim"), 0.0f);
	m_stateMachine.AddMotionEvent("LeftPunch", 0.35f, Event<>((Unitychan*)nullptr, &Unitychan::EndCommandBlocker));
	m_stateMachine.AddMotionEvent("LeftPunch", 0.4f, Event<>((Unitychan*)nullptr, &Unitychan::EndAnimation));

	m_stateMachine.AddState("RightHighKick", Motion::GetOrLoad("Data/bin/RightHighKick.anim"), 0.0f);
	m_stateMachine.AddMotionEvent("RightHighKick", 0.18f, Event<>((Unitychan*)nullptr, &Unitychan::ActionKickOnStartHitbox));
	m_stateMachine.AddMotionEvent("RightHighKick", 0.27f, Event<>((Unitychan*)nullptr, &Unitychan::ActionKickOnEndHitbox));
	m_stateMachine.AddMotionEvent("RightHighKick", 0.5f, Event<>((Unitychan*)nullptr, &Unitychan::EndCommandBlocker));
	m_stateMachine.AddMotionEvent("RightHighKick", 0.6f, Event<>((Unitychan*)nullptr, &Unitychan::EndAnimation));

	m_stateMachine.AddState("Down", Motion::GetOrLoad("Data/bin/Down.anim"), 0.0f);
	m_stateMachine.AddMotionEvent("Down", 0.73f, Event<>((Unitychan*)nullptr, &Unitychan::ActionRecover));

	m_stateMachine.AddState("HeadSpring", Motion::GetOrLoad("Data/bin/HeadSpring.anim"), 0.0f);
	m_stateMachine.AddMotionEvent("HeadSpring", 0.816f, Event<>((Unitychan*)nullptr, &Unitychan::EndCommandBlocker));
	m_stateMachine.AddMotionEvent("HeadSpring", 0.816f, Event<>((Unitychan*)nullptr, &Unitychan::EndAnimation));

	m_stateMachine.SetDefaultMotion("Idle");
}

void UnitychanDefinition::SetUnityChanMaterial()
{
	for (auto mesh : *(m_modelMeshSet))
	{
		if (mesh->m_name == "cheek")
		{
			mesh->m_material = new Material(
				"cheek", "UnityChan/Cheek",
				"Data/UnityChanTexture/cheek_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA
			);
		}
		else if (mesh->m_name == "skin")
		{
			mesh->m_material = new Material(
				"skin", "UnityChan/Skin",
				"Data/UnityChanTexture/skin_01.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "hair_front"
			|| mesh->m_name == "hair_frontside"
			|| mesh->m_name == "tail"
			|| mesh->m_name == "tail_bottom")
		{
			mesh->m_material = new Material(
				"hair", "Pose",
				"Data/UnityChanTexture/hair_01.png",
				"Data/UnityChanTexture/hair_01_NRM.png",
				"Data/UnityChanTexture/hair_01_SPEC.png",
				BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA
				, CULLMODE_NONE
			);
		}
		else if (mesh->m_name == "eye_base_old")
		{
			mesh->m_material = new Material(
				"eyeline", "Pose",
				"Data/UnityChanTexture/eyeline_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "BLW_DEF"
			|| mesh->m_name == "EL_DEF")
		{
			mesh->m_material = new Material(
				"eyeBlend", "UnityChan/FacialBlend",//
				"Data/UnityChanTexture/eyeline_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "EYE_DEF"
			|| mesh->m_name == "MTH_DEF")
		{
			mesh->m_material = new Material(
				"face", "UnityChan/FacialBlend",//
				"Data/UnityChanTexture/face_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "eye_L_old")
		{
			mesh->m_material = new Material(
				"eyeL", "Pose",
				"Data/UnityChanTexture/eye_iris_L_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "eye_R_old")
		{
			mesh->m_material = new Material(
				"eyeR", "Pose",
				"Data/UnityChanTexture/eye_iris_R_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}

		else if (mesh->m_name == "head_back")
		{
			mesh->m_material = new Material(
				"head_back", "Pose",
				"Data/UnityChanTexture/face_00.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png",
				"Data/UnityChanTexture/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "hairband")
		{
			mesh->m_material = new Material(
				"body", "Pose",
				"Data/UnityChanTexture/body_01.png",
				"Data/UnityChanTexture/body_01_NRM.png",
				"Data/UnityChanTexture/body_01_SPEC.png",
				BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA
				, CULLMODE_NONE
			);
		}
		else
		{
			mesh->m_material = new Material(
				"body", "Pose",
				"Data/UnityChanTexture/body_01.png",
				"Data/UnityChanTexture/body_01_NRM.png",
				"Data/UnityChanTexture/body_01_SPEC.png"
			);
		}
	}
}

void Unitychan::Update()
{
	CheckInputs();
	CheckAI();
	m_state->Update(/*&m_position*/);
	Entity::Update();
	//Console::Log("Actor Position: (" + std::to_string(m_position.x) + "," + std::to_string(m_position.z) + ")");
}

void Unitychan::Render() const
{
	auto& renderer = Game::GetInstance()->m_renderer;
	renderer.PushMatrix();
	//renderer.Scale(0.01f);
	//renderer.Translate3D(m_position);
	renderer.MultMatrix(m_pxActor->GetTransform());
	renderer.RotateY(m_rotation.y);
	m_state->EvaluateSkinning();
	std::vector<Matrix4>& skinningTransforms = m_state->m_skeletonInstance->m_currentSkinning.m_transforms;
	m_poseBuffer->Update(Game::GetInstance()->m_renderer.m_rhiContext, skinningTransforms.data());
	renderer.SetStructuredBuffer(3, m_poseBuffer);
	for (Mesh* mesh : *UnitychanDefinition::s_instance->m_modelMeshSet)
	{
		renderer.SetMaterial(mesh->m_material);
		renderer.DrawMesh(mesh);
	}
	renderer.PopMatrix();
}

Vector3 Unitychan::GetSocketPosition(const std::string& socketName)
{
	Matrix4 socketTransform = m_state->m_skeletonInstance->GetSocketTransform(socketName);
	Vector3 socketPosition;
	socketTransform = socketTransform * Matrix4::CreateRotationDegreesAboutY(m_rotation.y);
	socketPosition = socketTransform.GetPosition();

	return socketPosition + m_position;
}

Matrix4 Unitychan::GetSocketTransform(const std::string& socketName)
{
	Matrix4 socketTransform = m_state->m_skeletonInstance->GetSocketTransform(socketName);
	socketTransform = socketTransform * Matrix4::CreateRotationDegreesAboutY(m_rotation.y);
	socketTransform.SetTranslate(socketTransform.GetPosition() + m_position);
	return socketTransform;
}

void Unitychan::CheckInputs()
{
	if (!m_controller || m_controller->m_viewMode == Controller::VIEW_FREE)	
		return;
	
	if (m_isBlockingCommands)
	{
		m_velocity *= 0.5f;
		m_pxActor->SetLinearVelocity(m_velocity);
		return;
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_LMB))
	{
		ActionKick();
		return;
	}
	if (InputSystem::WasKeyJustPressed(KEYCODE_RMB))
	{
		m_isBlockingCommands = true;
		m_state->SetState("RightHighKick");
		return;
	}

	CheckMovementInputs();
}

void Unitychan::CheckMovementInputs()
{
	bool isPrevRunning = m_isRunning;
	m_isRunning = false;
	Vector3 direction;
	if (InputSystem::IsKeyDown('W'))
	{
		m_isRunning = true;
		direction += m_controller->m_camera.GetForward3D();
	}
	if (InputSystem::IsKeyDown('S'))
	{
		m_isRunning = true;
		direction -= m_controller->m_camera.GetForward3D();
	}
	if (InputSystem::IsKeyDown('A'))
	{
		m_isRunning = true;
		direction -= m_controller->m_camera.GetRight3D();
	}
	if (InputSystem::IsKeyDown('D'))
	{
		m_isRunning = true;
		direction += m_controller->m_camera.GetRight3D();
	}

	if (m_isRunning)
	{
		direction.y = 0;
		direction.Normalize();
		m_velocity = direction * m_runSpeed;
		m_pxActor->SetLinearVelocity(m_velocity);
		if (!isPrevRunning || m_state->m_currentState->name!="Run")
			m_state->SetState("Run");
	}
	else
	{
		m_velocity *= 0.86f;
		if (isPrevRunning)
			m_state->SetState("Idle");
		m_pxActor->SetLinearVelocity(m_velocity);
	}
	if (m_velocity != Vector3::ZERO && direction != Vector3::ZERO)
	{
		float newDegrees = Vector2(direction.z, direction.x).CalcHeadingDegrees();
		//float angleDistance = CalcShortestAngularDistance(m_rotation.y, newDegrees) * Time::deltaSeconds * 90.f;
		TurnToward(m_rotation.y, newDegrees, 720.f * Time::deltaSeconds);
	}
}

void Unitychan::EndCommandBlocker()
{
	m_isBlockingCommands = false;
}

void Unitychan::EndAnimation()
{
	m_state->SetState("Idle");
}

void Unitychan::Recover()
{
	m_isBlockingCommands = false;
	m_state->SetState("Idle");
}

void Unitychan::ActionKick()
{
	if (m_isBlockingCommands)
		return;
	m_isBlockingCommands = true;
	m_state->SetState("LeftPunch");
	m_isRunning = false;
	return;
}

void Unitychan::ActionRecover()
{
	m_state->SetState("HeadSpring");
}

void Unitychan::ActionKickOnStartHitbox()
{
	ParticleEffectDefinition* explosionDef = ParticleEffectDefinition::GetDefinition("explosion");
	ParticleEffect* effect = new ParticleEffect(explosionDef);
	ParticleEmitter* emitter = effect->m_emitters[0];
	Matrix4 toeTransform = GetSocketTransform("Character1_RightToeBase");
	emitter->m_transform = toeTransform;

	AudioSystem* audioIns = AudioSystem::GetInstance();
	audioIns->PlayAudio(audioIns->CreateOrGetSound("Data/audio/V0002.wav"));
	
	//dot
	const std::vector<Entity*>& entities = UnitychanActionStage::GetInstance()->m_entities;
	for (Entity* entity : entities)
	{
		if (entity == this)
			continue;
		Unitychan* unitychan = dynamic_cast<Unitychan*>(entity);
		if (unitychan)
		{
			float radius = 300;
			float angle = 60.f;
			Vector3 otherPosition = unitychan->m_position;
			Vector2 direction = Vector2::GetNormalizeFormDegrees(m_rotation.y);
			//Console::Log("rotation:"+std::to_string(m_rotation.y)+",direction:(" + std::to_string(direction.x) + "," + std::to_string(direction.y)+")");
			float dot = DotProduct((otherPosition - m_position).GetNormalize(), Vector3(direction.y,0.f, direction.x));
			float distanceSqrt = (otherPosition - m_position).CalcLengthSquared();
			if (dot > 0 && distanceSqrt < radius * radius && dot > CosDegrees(angle)/**/)
			{
				unitychan->m_state->SetState("Down");
				unitychan->m_rotation.y = m_rotation.y + 180.f;
				audioIns->PlayAudio(audioIns->CreateOrGetSound("Data/audio/V0003.wav"));
			}
		}
	}

	//sphere cast
	/*std::vector<PhysicsDynamicActor*> hitPxActors = 
		PhysicsSystem::SphereCast(UnitychanActionStage::GetInstance()->m_pxScene,toeTransform.GetPosition(), 
			500000.f, m_pxActor);

	for (auto hitPxActor : hitPxActors)
	{
		Entity* entity = (Entity*)hitPxActor->m_userData;
		if (entity && entity != this)
		{
			Unitychan* unitychan = dynamic_cast<Unitychan*>(entity);
			if (unitychan)
			{
				unitychan->m_state->SetState("Down");
				unitychan->m_rotation.y = m_rotation.y + 180.f;
			}
		}
	}*/

	//hitbox entity
	/*HitBox* hitBox = new HitBox();
	hitBox->m_onOverlap = Event<>*/
}

void Unitychan::ActionKickOnEndHitbox()
{

}

void Unitychan::CheckAI()
{
	if (m_controller && m_controller->m_viewMode == Controller::VIEW_UNITYCHAN)
		return;
	if (m_isBlockingCommands)
	{
		m_velocity *= 0.5f;
		m_pxActor->SetLinearVelocity(m_velocity);
		return;
	}
	else
	{
		m_velocity *= 0.5f;
		m_pxActor->SetLinearVelocity(m_velocity);
	}
}

Unitychan::Unitychan()
	: Entity()
	, m_definition(UnitychanDefinition::s_instance)
	, m_state(new StateMachineInstance(&UnitychanDefinition::s_instance->m_stateMachine, UnitychanDefinition::s_instance->m_skeleton))
	, m_isRunning(false)
	, m_isInAction(false)
	, m_isBlockingCommands(false)
{
	m_state->SetOwner(this);
	auto& renderer = Game::GetInstance()->m_renderer;
	m_poseBuffer = new StructuredBuffer(renderer.m_rhiDevice, m_state->m_skeletonInstance->m_currentPose.m_transforms.data(), sizeof(Matrix4), m_state->m_skeletonInstance->m_currentPose.m_transforms.size());
	renderer.SetStructuredBuffer(3, m_poseBuffer);
	
	m_pxActor = PhysicsSystem::CreateDynamicActor(Matrix4::CreateTranslation(Vector3(0.f, 0.f, 0.f)));
	m_pxActor->SetAngularLockX(true);
	m_pxActor->SetAngularLockY(true);
	m_pxActor->SetAngularLockZ(true);
	PhysicsShape* capsule = PhysicsSystem::CreateShapeCapsule(50,350, PhysicsMaterialParameters(0.5f,0.5f,0.f));
	Matrix4 capsuleTransfrom = Matrix4::CreateTransform(Vector3(0, 400, 0), Vector3(0, 0, 90), Vector3::ONE);
	capsule->SetTransform(capsuleTransfrom);
	m_pxActor->AddShape(capsule);
	m_pxActor->m_userData = this;
}

Unitychan::~Unitychan()
{
	SAFE_DELETE(m_state);
	SAFE_DELETE(m_poseBuffer);
}
