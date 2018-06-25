#include "Game.hpp"
#include "Engine\Core\Config.hpp"
#include "GameConfig.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "Engine\Core\Window.hpp"
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\RHI\Texture2D.hpp"
#include "Engine\RHI\Sampler.hpp"
#include "Engine\RHI\ConstantBuffer.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Math\AABB3.hpp"
#include "Engine\RHI\VertexBuffer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <math.h>
#include "Engine\Core\RGBAf.hpp"
#include "Engine\Tools\fbx.hpp"
#include <thread>
#include "Engine\RHI\StructuredBuffer.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Math\Matrix4.hpp"
#include "Engine\RHI\RasterState.hpp"
#include "Engine\Tools\Memory.hpp"
#include "Engine\Core\Profiler.hpp"
#include "Engine\Math\Physics.hpp"
#include "Scene.hpp"
#include "Engine\Math\PhysicsSpringJoint.hpp"

Game* Game::s_instance;



Game::Game()
	: m_quitReason(QUITREASON_NONE)
	, m_timeOfLastRunframe(0.f)
	, m_camera()
	, m_audio()
{
}

Game::~Game()
{
}

void Game::AssignConsoleCommands()
{
	Console::AssignCommand("full_screen", ToggleFullScreen);
	Console::AssignCommand("full_screen_borderless", ToggleFullScreenBorderless);

	Console::AssignCommand("set_texture", ChangeTexture);
	Console::AssignCommand("set_shader", ChangeShader);
	Console::AssignCommand("quit", S_Quit);
	Console::AssignCommand("set_fov", SetFov);

	Console::AssignCommand("set_show_grid", SetShowGrid);
	Console::AssignCommand("set_show_axis", SetShowAxis);

	Console::AssignCommand("load_mesh", LoadMeshes);
	Console::AssignCommand("save_mesh", SaveMeshes);
	Console::AssignCommand("delete_mesh", DeleteMeshes);
	Console::AssignCommand("load_meshes", LoadMeshes);
	Console::AssignCommand("save_meshes", SaveMeshes);
	Console::AssignCommand("delete_meshes", DeleteMeshes);
	Console::AssignCommand("load_skeleton", LoadSkeleton);
	Console::AssignCommand("save_skeleton", SaveSkeleton);
	Console::AssignCommand("delete_skeleton", DeleteSkeleton);
	Console::AssignCommand("load_motion", LoadMotion);
	Console::AssignCommand("save_motion", SaveMotion);
	Console::AssignCommand("delete_motion", DeleteMotion);
	Console::AssignCommand("load_hand_motion", LoadHandMotion);
	Console::AssignCommand("save_hand_motion", SaveHandMotion);
	Console::AssignCommand("delete_hand_motion", DeleteHandMotion);

	Console::AssignCommand("set_camera_tracking", SetCameraTracking);
	Console::AssignCommand("SCT", SetCameraTracking);

	Console::AssignCommand("load_skeleton_mesh", LoadSkeletonMesh);

	Console::AssignCommand("set_unitychan_material", SetUnityChanMaterial);
	Console::AssignCommand("set_unitychan_expression", SetUnityChanFacialExpression);

	Console::AssignCommand("ue_default", SetUnityChanExpression_Default);
	Console::AssignCommand("ue_eyeclose", SetUnityChanExpression_EyeClose);
	Console::AssignCommand("ue_conf", SetUnityChanExpression_Conf);
	Console::AssignCommand("ue_smile", SetUnityChanExpression_Smile3);
	Console::AssignCommand("ue_smile4", SetUnityChanExpression_Smile4);

	Console::AssignCommand("play_scene", PlayScene);
	Console::AssignCommand("stop_scene", StopScene);
}

void Game::Run()
{
	Start();
	while (IsRunning()) {
		RunFrame();
	}
	End();
}

void Game::Start()
{

	InitRendering();
	BuildVbo();
	
	m_fov = 60.f;

	int screenWidth;
	int screenHeight;

	Config::GetInt(screenWidth, "window_width");
	Config::GetInt(screenHeight, "window_height");
	Config::GetFloat(m_fov, "field_of_view");

	m_renderer.SetupConsole();
	Matrix4::YAW_AXIS = Vector3(0.0f, 1.0f, 0.0f);
	Matrix4::PITCH_AXIS = Vector3(1.0f, 0.0f, 0.0f);
	Matrix4::ROLL_AXIS = Vector3(0.0f, 0.0f, 1.0f);
	//Console::Setup(&m_renderer);
	Console::SetPosition(Vector2(screenWidth * 0.01f, screenHeight * 0.02f));
	Console::SetScale(2.f);

	AssignConsoleCommands();
	ParticleSystem::Start();

	std::string image = "Test_StbiAndDirectX.png";
	Config::GetString(image, "image");
	m_texSample = new Texture2D(m_renderer.m_rhiDevice, "Data/Images/" + image);

	Config::GetString(image, "image_normal");
	m_texNormal = new Texture2D(m_renderer.m_rhiDevice, "Data/Images/" + image);

	Config::GetString(image, "image_spec");
	m_texSpec = new Texture2D(m_renderer.m_rhiDevice, "Data/Images/" + image);

	std::string image2 = "Test_StbiAndDirectX.png";
	Config::GetString(image2, "image2");
	m_texSample2 = new Texture2D(m_renderer.m_rhiDevice, "Data/Images/" + image2);

	//m_sampler = new Sampler(m_renderer.m_rhiDevice, FILTER_POINT, FILTER_LINEAR);

	time.time = 0.0f;
	time.screenWidth = (float)screenWidth;
	time.screenHeight = (float)screenHeight;
	time.shaderParameter = 1.f;
	time_constants = new ConstantBuffer(m_renderer.m_rhiDevice, &time, sizeof(time));
	m_renderer.SetConstantBuffer(1, time_constants);

// 	m_light.AMBIENT_FACTOR = Vector4::ONE;
// 	m_light.LIGHT_COLOR = Vector4::ONE;
// 	m_light.LIGHT_POSITION = Vector4::ONE;
// 	m_light.LIGHT_DIRECTION = Vector4::ONE;
// 	m_light.ATTENUATION = Vector4::ONE;
// 	m_lightBuffer = new ConstantBuffer(m_renderer.m_rhiDevice, &m_light, sizeof(m_light));


	m_camera.m_isControllableByKeyboard = true;

	m_doesDrawAxis = false;
	m_doesDrawGrid = true;

	m_centerOfLights = Vector3();
	m_rotationSpeedOfLights = 30.f;
	m_currentRotationOfLights = 0.f;
	m_radiusOfLights = 2.5f;

	m_camera.m_position.y = 1.2f;
	m_camera.m_position.z = 4.f;
	m_camera.m_rotation.x = 10.f;
	m_camera.m_rotation.y = 180.f;

	CameraZXY::s_moveSpeed = 1.f;

	m_meshFilePath =
		"Data/bin/unitychan.mesh";
		//"Data/bin/unitychan_lipsync.mesh";
		/*"Data/bin/robot_mesh.bin";*/
		/*"Data/mesh/Drotek.fbx";*/
		//"Data/mesh/CandyRockStar.fbx";
		//"Data/UnityChan/unitychan.fbx";
		//"Data/bin/unitychan.mesh";
		//"Data/bin/unitychan_candy.mesh";
	m_skeletonFilePath = 
		//"Data/mesh/CandyRockStar.fbx";
		//"Data/UnityChan/Models/unitychan.fbx";
		//"Data/UnityChan/unitychan_lipsync.fbx";
		"Data/bin/unitychan.skel";
		//"Data/bin/unitychan_lipsync.skel";
		//"Data/bin/unitychan_candy.skel";
	m_motionPath =
		//"";
		//"Data/bin/unitychan_win00.anim";
		"Data/bin/unit_in_the_sky.anim";
		//"Data/bin/unit_in_the_sky_candy.anim";
		//"Data/bin/unit_in_the_sky_hand.anim";
		/*"Data/mesh/Drotek.fbx";*/
		/*"Data/mesh/SampleBox.fbx";*/
		//"Data/animation/Unit_in_the_sky.fbx";
		//"Data/UnityChan/Animations/unitychan_win00.fbx";
	m_handMotionPath =
		//"Data/animation/Hand_expression.fbx";
		"Data/bin/unit_in_the_sky_hand.anim";
		//"Data/bin/unit_in_the_sky_hand_candy.anim";
	m_animationFilePath = 
		//"";
		"Data/animation/LipSync.fbx";
		/*"Data/mesh/SampleBox.fbx";*/
		/*"Data/animation/Unit_in_the_sky.fbx";*/
		/*"Data/UnityChan/Animations/unitychan_win00.fbx";*/
	m_blendShapeAnimFilePath = "";
		//"Data/animation/LipSync.fbx";

	FBXUtils::Setup();

	ParticleEffectDefinition::LoadDefinition("Data/xml/particle.xml");
	ParticleEffectDefinition::LoadDefinition("Data/xml/particleL.xml");
	ParticleEffectDefinition::LoadDefinition("Data/xml/particleR.xml");
	ParticleEffectDefinition::LoadDefinition("Data/xml/ribbon.xml");
	ParticleEffectDefinition::LoadDefinition("Data/xml/ribbonL.xml");
	ParticleEffectDefinition::LoadDefinition("Data/xml/ribbonR.xml");

	m_leftHandRibbonEffect = new ParticleEffect("ribbonL");//new ParticleEffect("sharps");
	m_rightHandRibbonEffect = new ParticleEffect("ribbonR");//new ParticleEffect("sharps");
	
	m_leftHandParticleEffect = new ParticleEffect("sharpsL");
	m_rightHandParticleEffect = new ParticleEffect("sharpsR");

	m_facialExpressNameForUnityChan = "";

	m_modelMesh = nullptr;
	m_modelMeshSet = nullptr;
	m_skeleton = nullptr;
	m_skeletonInstance = nullptr;
	m_motion = nullptr;
	m_handMotion = nullptr;
	m_animation = nullptr;
	m_poseBuffer = nullptr;
	m_bgmChannel = nullptr;
	m_facialAnimations = nullptr;

	m_MTHConstantBuffer = nullptr;
	m_MTHWeightStructedBuffer = nullptr;
	m_MTHShapesStructedBuffer = nullptr;
	m_MTHBlendInstance	= nullptr;

	m_BLWConstantBuffer = nullptr;
	m_BLWWeightStructedBuffer = nullptr;
	m_BLWShapesStructedBuffer = nullptr;
	m_BLWBlendInstance = nullptr;

	m_EYEConstantBuffer = nullptr;
	m_EYEWeightStructedBuffer = nullptr;
	m_EYEShapesStructedBuffer = nullptr;
	m_EYEBlendInstance = nullptr;

	m_ELConstantBuffer = nullptr;
	m_ELWeightStructedBuffer = nullptr;
	m_ELShapesStructedBuffer = nullptr;
	m_ELBlendInstance = nullptr;

	m_spectrum.resize(1024, 0.f);
	m_spectrumBuffer = new StructuredBuffer(m_renderer.m_rhiDevice, m_spectrum.data(), sizeof(float), m_spectrum.size());
	m_renderer.SetStructuredBuffer(10, m_spectrumBuffer);
	m_modelPosition.position = Vector3();
	m_modelPositionBuffer = new ConstantBuffer(m_renderer.m_rhiDevice, &m_modelPosition, sizeof(model_position_t));
	m_renderer.SetConstantBuffer(10, m_modelPositionBuffer);

	m_doesCameraTraceModel = false;

	m_startPlaying = false;
	m_playTime = 0.f;

	m_sound = m_audio.CreateOrGetSound("Data/audio/Unite In The Sky (full).mp3");

	LoadMeshAndMotionStatic();
	SetUnityChanMaterial();
	//SetUnityChanCandyMaterial();
	SetUnityBlendShape();
	SetUnityChanFacialAnimation();
	SetUnityChanSpringJoints();

	PhysicsSystem::StartUp();
	m_scene.Start();

	Time::Update();
	Time::Update();
}



void Game::RunFrame()
{
	// Process Window Messages
	//ClockSystemStep(); 
	//InputSystem::GetInstance()->BeginFrame();
	// update the frame time


	float currentTime = (float)GetCurrentTimeSeconds();
	float deltaSeconds = currentTime - m_timeOfLastRunframe;

	m_timeOfLastRunframe = currentTime;

	//m_renderer.GetWindow()->LockMouse();
	m_renderer.GetWindow()->RunMessagePump();

	

	//DebuggerPrintf("Time: %f\n", time.time);

	if (m_renderer.IsClosed()) {
		Quit();
	}
	else {
		Update(deltaSeconds);
		Render();
	}

	InputSystem::GetInstance()->EndFrame();
}

void Game::End()
{
	CleanupRendering();
}

void Game::Update(float deltaSeconds)
{
	Time::Update();
	deltaSeconds = Time::deltaSeconds;
	if (Console::IsOpen())
	{
		m_camera.m_isControllableByKeyboard = false;
		Console::GetInput();
	}
	else
	{
		m_camera.m_isControllableByKeyboard = true;
		CheckInput(deltaSeconds);
	}
	m_camera.Update(deltaSeconds);

	if (time.shaderParameter > 1.f)
		time.shaderParameter = 1.f;
	else if (time.shaderParameter < 0.f)
		time.shaderParameter = 0.f;

	time.time += deltaSeconds;
	time_constants->Update(m_renderer.m_rhiContext, &time);

	//m_centerOfLights = Vector3(cos(time.time) * 2.f, sin(time.time * 1.f) * 3.f + 1.f, 1.5f);
	m_currentRotationOfLights += m_rotationSpeedOfLights * deltaSeconds;

	UpdateSkeleton(deltaSeconds);
	UpdateUnitychanBlendShape();
	m_audio.Update();

	if(m_startPlaying)
		ParticleSystem::Update();

	if (m_bgmChannel)
	{
		m_audio.GetSpectrum(m_bgmChannel, m_spectrum);
		m_spectrumBuffer->Update(m_renderer.m_rhiContext, m_spectrum.data());
	}
	if(m_usePhysxForSpringJoins)
		m_scene.Update();
}

void Game::UpdateSkeleton(float deltaSeconds)
{
	if (m_skeletonInstance && (m_motion))
	{
		//static float time = 0.f;
		float& time = m_playTime;
		if (m_startPlaying)
			time += deltaSeconds;
		
		m_skeletonInstance->ApplyTimedMotionAdditive(m_motion, time);
		if(m_handMotion)
			m_skeletonInstance->ApplyTimedMotionAdditive(m_handMotion, time);
		
		//if(!InputSystem::IsKeyDown('J'))
		if (m_startPlaying)
		{
			if(!m_usePhysxForSpringJoins)
				UpdateSpringJoints();
			else
			{
				m_scene.m_lHairSpringJoint->Update(m_skeletonInstance->m_currentPose);
				m_scene.m_rHairSpringJoint->Update(m_skeletonInstance->m_currentPose);
			}
		}
			

		

		m_skeletonInstance->EvaluateSkinning();

		std::vector<Matrix4>& skinningTransforms = m_skeletonInstance->m_currentSkinning.m_transforms;
		m_poseBuffer->Update(m_renderer.m_rhiContext, skinningTransforms.data());
		Vector3 socketPosition = m_skeletonInstance->GetSocketTransform("Character1_Hips").GetPosition() * 0.01f;
		m_modelPosition.position = socketPosition;
		m_modelPositionBuffer->Update(m_renderer.m_rhiContext, &m_modelPosition);
		if (m_doesCameraTraceModel)
		{
			
			Vector3 cameraTrackPosition = socketPosition + Vector3(0.f, 0.0f, 2.f);
			m_camera.m_position.x = cameraTrackPosition.x;
			m_camera.m_position.z = cameraTrackPosition.z;
			//m_camera.m_position = cameraTrackPosition;
		}
		Vector3 leftHandPos = m_skeletonInstance->GetSocketTransform("Character1_LeftHandMiddle1").GetPosition() * 0.01f;
		Vector3 rightHandPos = m_skeletonInstance->GetSocketTransform("Character1_RightHandMiddle1").GetPosition() * 0.01f;
		m_leftHandRibbonEffect->m_emitters[0]->m_transform = Matrix4::CreateTranslation(leftHandPos);
		m_rightHandRibbonEffect->m_emitters[0]->m_transform = Matrix4::CreateTranslation(rightHandPos);
		m_leftHandParticleEffect->m_emitters[0]->m_transform = Matrix4::CreateTranslation(leftHandPos);
		m_rightHandParticleEffect->m_emitters[0]->m_transform = Matrix4::CreateTranslation(rightHandPos);
	}
		
}

void Game::UpdateSpringJoints()
{
	for (size_t index = 0; index < m_springs.size(); index++)
		m_springs[index].Update();
}

void Game::UpdateUnitychanBlendShape()
{
	if (!m_MTHBlendInstance)
		return;
	if (m_modelMeshSet)
	{
		
		m_MTHBlendInstance->SetBlendWeight("MTH_A", 0.f);
		m_MTHBlendInstance->SetBlendWeight("MTH_E", 0.f);
		m_MTHBlendInstance->SetBlendWeight("MTH_I", 0.f);
		m_MTHBlendInstance->SetBlendWeight("MTH_O", 0.f);
		m_MTHBlendInstance->SetBlendWeight("MTH_U", 0.f);

		if (!m_startPlaying)
		{
			//m_MTHBlendInstance->ResetBlendWeight();
			//m_EYEBlendInstance->ResetBlendWeight();
			//m_BLWBlendInstance->ResetBlendWeight();
			//m_ELBlendInstance->ResetBlendWeight();

			m_MTHBlendInstance->SetBlendWeight("MTH_" + m_facialExpressNameForUnityChan, 100);
			m_EYEBlendInstance->SetBlendWeight("EYE_" + m_facialExpressNameForUnityChan, 100);
			m_BLWBlendInstance->SetBlendWeight("BLW_" + m_facialExpressNameForUnityChan, 100);
			m_ELBlendInstance->SetBlendWeight("EYE_" + m_facialExpressNameForUnityChan, 100);
		}
		else
		{
			m_faceChangeEvent.Update();
		}

		
		
		if(m_animation)
			UpdateUnitychanLipSync();

		m_MTHWeightStructedBuffer->Update(m_renderer.m_rhiContext, m_MTHBlendInstance->m_weights.data());
		m_EYEWeightStructedBuffer->Update(m_renderer.m_rhiContext, m_EYEBlendInstance->m_weights.data());
		m_BLWWeightStructedBuffer->Update(m_renderer.m_rhiContext, m_BLWBlendInstance->m_weights.data());
		m_ELWeightStructedBuffer->Update(m_renderer.m_rhiContext, m_ELBlendInstance->m_weights.data());
	}
}

void Game::UpdateUnitychanLipSync()
{
	float total = 100.f;
	float aWeight = 0.f;
	float eWeight = 0.f;
	float iWeight = 0.f;
	float oWeight = 0.f;
	float uWeight = 0.f;
 	aWeight = m_animation->GetCurveByName("rsync_A").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * 100.f;
 	eWeight = m_animation->GetCurveByName("rsync_E").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * 100.f;
 	iWeight = m_animation->GetCurveByName("rsync_I").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * 100.f;
 	oWeight = m_animation->GetCurveByName("rsync_O").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * 100.f;
 	uWeight = m_animation->GetCurveByName("rsync_U").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * 100.f;
	//aWeight = m_animation->GetCurveByName("rsync_A").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * total;
	//total -= aWeight;
	//iWeight = m_animation->GetCurveByName("rsync_I").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * total;
	//total -= iWeight;
	//uWeight = m_animation->GetCurveByName("rsync_U").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * total;
	//total -= uWeight;
	//eWeight = m_animation->GetCurveByName("rsync_E").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * total;
	//total -= eWeight;
	//oWeight = m_animation->GetCurveByName("rsync_O").EvaluateKeySet(m_playTime)[AnimCurve::POS_Z] * total;

	aWeight = aWeight >= 0 ? aWeight : 0;
	eWeight = eWeight >= 0 ? eWeight : 0;
	iWeight = iWeight >= 0 ? iWeight : 0;
	oWeight = oWeight >= 0 ? oWeight : 0;
	uWeight = uWeight >= 0 ? uWeight : 0;
	if (aWeight || eWeight || iWeight || oWeight || uWeight)
	{
		//m_MTHBlendInstance->ResetBlendWeight();
		m_MTHBlendInstance->SetBlendWeight("MTH_A", aWeight);
		m_MTHBlendInstance->SetBlendWeight("MTH_E", eWeight);
		m_MTHBlendInstance->SetBlendWeight("MTH_I", iWeight);
		m_MTHBlendInstance->SetBlendWeight("MTH_O", oWeight);
		m_MTHBlendInstance->SetBlendWeight("MTH_U", uWeight);
	}
}

void Game::RenderUnitychan()
{
	for (Mesh* mesh : *m_modelMeshSet)
	{
		if (mesh->m_name == "MTH_DEF")
		{
			m_renderer.SetStructuredBuffer(4, m_MTHShapesStructedBuffer);
			m_renderer.SetStructuredBuffer(5, m_MTHWeightStructedBuffer);
			m_renderer.SetConstantBuffer(5,	m_MTHConstantBuffer);
		}
		else if (mesh->m_name == "EYE_DEF")
		{
			m_renderer.SetStructuredBuffer(4, m_EYEShapesStructedBuffer);
			m_renderer.SetStructuredBuffer(5, m_EYEWeightStructedBuffer);
			m_renderer.SetConstantBuffer(5, m_EYEConstantBuffer);
		}
		else if (mesh->m_name == "BLW_DEF")
		{
			m_renderer.SetStructuredBuffer(4, m_BLWShapesStructedBuffer);
			m_renderer.SetStructuredBuffer(5, m_BLWWeightStructedBuffer);
			m_renderer.SetConstantBuffer(5, m_BLWConstantBuffer);
		}
		else if (mesh->m_name == "EL_DEF")
		{
			m_renderer.SetStructuredBuffer(4, m_ELShapesStructedBuffer);
			m_renderer.SetStructuredBuffer(5, m_ELWeightStructedBuffer);
			m_renderer.SetConstantBuffer(5, m_ELConstantBuffer);
		}
		else
		{
			m_renderer.SetStructuredBuffer(4, nullptr);
			m_renderer.SetStructuredBuffer(5, nullptr);
			m_renderer.SetConstantBuffer(5, nullptr);
		}
		//m_renderer.SetMaterial(mesh->m_material);
		m_renderer.SetMaterial(mesh->m_material);
		m_renderer.DrawMesh(mesh);
	}
	
}

void Game::SetUnityChanSpringJoints()
{
	//Left tail
	//m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_00"), m_skeletonInstance->GetJoint("J_L_HairTail_01"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_01"), m_skeletonInstance->GetJoint("J_L_HairTail_02"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_02"), m_skeletonInstance->GetJoint("J_L_HairTail_03"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_03"), m_skeletonInstance->GetJoint("J_L_HairTail_04"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_04"), m_skeletonInstance->GetJoint("J_L_HairTail_05"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_L_HairTail_05"), m_skeletonInstance->GetJoint("J_L_HairTail_06"), m_skeletonInstance));
	//Right tail
	//m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_00"), m_skeletonInstance->GetJoint("J_R_HairTail_01"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_01"), m_skeletonInstance->GetJoint("J_R_HairTail_02"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_02"), m_skeletonInstance->GetJoint("J_R_HairTail_03"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_03"), m_skeletonInstance->GetJoint("J_R_HairTail_04"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_04"), m_skeletonInstance->GetJoint("J_R_HairTail_05"), m_skeletonInstance));
	m_springs.push_back(SpringJoint(m_skeletonInstance->GetJoint("J_R_HairTail_05"), m_skeletonInstance->GetJoint("J_R_HairTail_06"), m_skeletonInstance));

}

void Game::Render()
{
	m_renderer.SetRenderTarget(nullptr);
	m_renderer.ClearColor(0x222222ff);
	m_renderer.ClearDepth();

	m_renderer.SetViewport();
	m_renderer.EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
	m_renderer.SetSampler(FILTER_POINT, FILTER_LINEAR);
	m_renderer.SetShader(m_currentShaderName);
	m_renderer.SetAmbientLight(Rgba::WHITE, 4.f);
	m_renderer.DisableAllPointLight();

	const IntVector2& windowSize = m_renderer.GetWindowSize();
	float aspectRatio = (float)windowSize.x / (float)windowSize.y;

	m_renderer.EnableDepthTest();
	m_renderer.EnableDepthWrite();

	m_renderer.SetEyePosition(m_camera.m_position);
	m_renderer.SetViewMatrix(m_camera.GetViewMatrix());
	m_renderer.SetPerspectiveProjection(m_fov, aspectRatio, 0.1f, 50.0f);
	m_renderer.SetAmbientLight(Rgba::WHITE, 0.8f);
	//m_renderer.EnablePointLight(1,m_centerOfLights, Rgba::WHITE, 1.f, Vector4(0.f,0.f,0.1f,1.f), Vector4(0.f, 0.f, 0.1f, 1.f));
	m_renderer.SetSpecularConstants(32.0f, 1.0f);
	SetAndDrawLights();
	//m_renderer.SetTexture(m_texSample);
	m_renderer.SetDiffuseTexture(m_texSample);
	m_renderer.SetNormalTexture(m_texNormal);
	//m_renderer.SetNormalTexture(nullptr);
	m_renderer.SetSpecularTexture(m_texSpec);
	//m_renderer.DrawVbo(PRIMITIVE_TRIANGLES, quadVbo);

// 	m_renderer.PushMatrix();
// 	m_renderer.RotateY(m_currentRotationOfLights * 1.f);
// 	m_renderer.DrawVbo(PRIMITIVE_TRIANGLES, cubeVbo);
// 	m_renderer.PopMatrix();

	//m_renderer.SetTexture(m_texSample2);
	m_renderer.SetTexture(nullptr);
	m_renderer.SetNormalTexture(nullptr);

	m_renderer.PushMatrix();
	m_renderer.Translate(Vector3(-4.f, 1.f, 0.f));
	//m_renderer.DrawVbo(PRIMITIVE_TRIANGLES, sphereVbo);
	//m_renderer.DrawVbo(PRIMITIVE_LINES, sphereTbnLines);
	m_renderer.PopMatrix();

	//m_renderer.DrawVbo(PRIMITIVE_TRIANGLES, cylinderVbo);
	//m_renderer.DrawVbo(PRIMITIVE_LINES, cylinderTbnLines);
	
	
	m_renderer.SetAmbientLight(Rgba::WHITE, 0.8f);
	//m_renderer.DisableAllPointLight();
	m_renderer.SetShader(nullptr);
	if (m_doesDrawAxis)
		m_renderer.DrawVbo(PRIMITIVE_LINES, axisVbo);
	if (m_doesDrawGrid)
	{
		if (m_bgmChannel)
			m_renderer.SetShader("MusicFloor");
		m_renderer.DrawVbo(PRIMITIVE_LINES, gridVbo);
	}

	m_renderer.DrawDebugLines();
	
	if (m_skeleton || m_modelMesh || m_skeletonInstance || m_motion) {
		m_renderer.PushMatrix();


		m_renderer.Scale(0.01f);
		//m_renderer.Translate(Vector3(0.f, 0.f, 10.f));
		m_renderer.Translate(Vector3(0.f, 0.f, 400.f));
		//if(m_skeleton)
		//	m_renderer.DrawSkeleton(m_skeleton);
		//m_renderer.Translate(Vector3(0.f, 0.f, -5.f));
		m_renderer.Translate(Vector3(0.f, 0.f, -200.f));
		// 		m_renderer.Scale(2.f);
		//  		if(m_skeletonInstance)
		//  			m_renderer.DrawSkeletonInstance(m_skeletonInstance);
		// 		m_renderer.Scale(0.5f);
		//m_renderer.Translate(Vector3(0.f, 0.f, -5.f));
		m_renderer.Translate(Vector3(0.f, 0.f, -200.f));
		if (m_motion || m_animation)
			m_renderer.SetShader("Pose");
		//if (m_modelMesh)
		//	m_renderer.DrawMesh(m_modelMesh);

		if (m_modelMeshSet)
			RenderUnitychan();
		if (InputSystem::IsKeyDown('J') && !m_usePhysxForSpringJoins)//(false)//(true)//
		{
			m_renderer.SetMaterial("Default");
			for (auto& springJoint : m_springs)
			{
				m_renderer.PushMatrix();
				m_renderer.Translate3D(springJoint.m_currTipPos);
				m_renderer.DrawVbo(sphereVbo);
				m_renderer.PopMatrix();
			}
		}
		
		m_renderer.SetShader(nullptr);
		m_renderer.PopMatrix();
	}

	if(InputSystem::IsKeyDown('J') && m_usePhysxForSpringJoins)
		m_scene.Render();

	m_renderer.DrawParticleSystem(m_camera.m_position);

	if (Console::IsOpen())
		m_renderer.DrawConsole();
		//Console::Render();

	
	m_renderer.DisableBlend();

	m_renderer.Present();
}

void Game::SetAndDrawLights()
{
	const int MAX_NUM_LIGHT = 2;
	float deltaDegrees = 360.f / float(MAX_NUM_LIGHT);
	float currentDegree = 0.f;
	for (int i = 0; i < MAX_NUM_LIGHT; i++, currentDegree += deltaDegrees)
	{
		float trueDegree = /*m_currentRotationOfLights +*/ 30.f + currentDegree;
		Vector3 offset(CosDegrees(trueDegree), 0.f, SinDegrees(trueDegree));
		offset *= m_radiusOfLights;
		offset.y += 1.f;
		float scale = 1.f / (float)i;
		float rScale = fabs(1.f - scale);
		float gScale = fabs(rScale - scale);
		float bScale = fabs(1.f - gScale);
		RgbaF colorf(rScale, gScale, bScale);
		Rgba color(colorf);

		switch (i)
		{
		case 0:
			color = Rgba::WHITE; break;
		case 1:
			color = Rgba::WHITE; break;
		case 2:
			color = Rgba::BLUE; break;
		case 3:
			color = Rgba::YELLOW; break;
		case 4:
			color = Rgba::RED; break;
		case 5:
			color = Rgba::GREEN; break;
		case 6:
			color = Rgba::BLUE; break;
		case 7:
			color = Rgba::YELLOW; break;
		default:
			break;
		}


		m_renderer.EnablePointLight(i, m_centerOfLights + offset, color, 1.f, 
			Vector4(1.f, 0.f, 0.f, 0.f), Vector4(16.f, 16.f, 16.f, 0.f));
	}
	currentDegree = 0.f;
	m_renderer.SetTexture(nullptr);
	m_renderer.SetNormalTexture(nullptr);
	for (int i = 0; i < MAX_NUM_LIGHT; i++, currentDegree += deltaDegrees)
	{
		float trueDegree = m_currentRotationOfLights + currentDegree;
		Vector3 offset(CosDegrees(trueDegree), 0.f, SinDegrees(trueDegree));
		offset *= m_radiusOfLights;
		offset.y += 1.f;
		Vector3 spherePosition = m_centerOfLights + offset;
		m_renderer.PushMatrix();
		//m_renderer.Scale(Vector3(0.5f, 0.5f, 0.5f));
		m_renderer.Translate(spherePosition);
		//m_renderer.DrawVbo(PRIMITIVE_QUADS, sphereVbo);
		m_renderer.PopMatrix();
	}
	m_renderer.SetSpecularConstants(64, 128);
}

void Game::Quit(eQuitReason reason)
{
	m_quitReason = reason;
}

void Game::CheckInput(float deltaSeconds)
{
	if (!Console::IsOpen() && InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_ENTER))
		Console::Open();
	else if (InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_GRAVE))
		Console::Toggle();
	else if (InputSystem::GetInstance()->WasKeyJustPressed(KEYCODE_ESCAPE))
		Quit();
	else if (InputSystem::GetInstance()->WasKeyJustPressed('U'))
		m_renderer.GetWindow()->TriggerFullScreen();
	else if (InputSystem::GetInstance()->WasKeyJustPressed('O'))
		m_camera = CameraZXY();
	else if (InputSystem::GetInstance()->IsKeyDown(KEYCODE_LMB))
		m_fov -= deltaSeconds * 10.f;
		//m_renderer.GetWindow()->BorderlessFullScreen();
	else if (InputSystem::GetInstance()->IsKeyDown(KEYCODE_RMB))
		m_fov += deltaSeconds * 10.f;
		//m_renderer.GetWindow()->CenterWindow();
	else if (InputSystem::GetInstance()->WasKeyJustPressed('T'))
		m_renderer.GetWindow()->SetWindowTitle("WENZHENG HUANG : SD3 ASSIGNMENT 2");
	else if (InputSystem::GetInstance()->IsKeyDown(KEYCODE_UP))
		time.shaderParameter += 0.5f * deltaSeconds;
	else if (InputSystem::GetInstance()->IsKeyDown(KEYCODE_DOWN))
		time.shaderParameter -= 0.5f * deltaSeconds;

	if (m_fov > 120.f)
		m_fov = 120.f;
}

void Game::ToggleFullScreen(const std::string& /*= ""*/)
{
	s_instance->m_renderer.m_rhiOutput->m_window->TriggerFullScreen();
}

void Game::ToggleFullScreenBorderless(const std::string& /*= ""*/)
{
	s_instance->m_renderer.m_rhiOutput->m_window->BorderlessFullScreen();
}

void Game::SetUnityChanMaterial(const std::string& /*= ""*/)
{
	for (auto mesh : *(s_instance->m_modelMeshSet))
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
				,CULLMODE_NONE
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

void Game::SetUnityChanCandyMaterial(const std::string& /*= ""*/)
{
	for (auto mesh : *(s_instance->m_modelMeshSet))
	{
		if (mesh->m_name == "cheek")
		{
			mesh->m_material = new Material(
				"cheek", "UnityChan/Cheek",
				"Data/mesh/CandyTextures/cheek_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA
			);
		}
		else if (mesh->m_name == "Skin")
		{
			mesh->m_material = new Material(
				"skin", "Pose",
				"Data/mesh/CandyTextures/skin_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "HairBack_DS_new01"
			|| mesh->m_name == "HairFront_DS_new01"
			|| mesh->m_name == "HairSide_DS_new01")
		{
			mesh->m_material = new Material(
				"hair", "Pose",
				"Data/mesh/CandyTextures/hair_01.png",
				"Data/mesh/CandyTextures/hair_01_NRM.png",
				"Data/mesh/CandyTextures/hair_01_SPEC.png"
			);
		}
		else if (mesh->m_name == "eye_base_old")
		{
			mesh->m_material = new Material(
				"eyeline", "Pose",
				"Data/mesh/CandyTextures/eyeline_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "BLW_DEF"
			|| mesh->m_name == "EL_DEF")
		{
			mesh->m_material = new Material(
				"eyeBlend", "UnityChan/FacialBlend",//
				"Data/mesh/CandyTextures/eyeline_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "EYE_DEF"
			|| mesh->m_name == "MTH_DEF")
		{
			mesh->m_material = new Material(
				"face", "UnityChan/FacialBlend",//
				"Data/mesh/CandyTextures/face_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "eye_L_old")
		{
			mesh->m_material = new Material(
				"eyeL", "Pose",
				"Data/mesh/CandyTextures/eye_iris_L_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "eye_R_old")
		{
			mesh->m_material = new Material(
				"eyeR", "Pose",
				"Data/mesh/CandyTextures/eye_iris_R_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}

		else if (mesh->m_name == "head_back")
		{
			mesh->m_material = new Material(
				"head_back", "Pose",
				"Data/mesh/CandyTextures/face_00.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png",
				"Data/mesh/CandyTextures/DEFAULT_NORMAL.png"
			);
		}
		else if (mesh->m_name == "Effector_new01")
		{
			mesh->m_material = new Material(
				"effector", "Pose",
				"Data/mesh/CandyTextures/body_01.png",
				"Data/mesh/CandyTextures/body_01_NRM.png",
				"Data/mesh/CandyTextures/body_01_SPEC.png"
			);
		}
		else
		{
			mesh->m_material = new Material(
				"body", "Pose",
				"Data/mesh/CandyTextures/body_00.png",
				"Data/mesh/CandyTextures/body_00_NRM.png",
				"Data/mesh/CandyTextures/body_00_SPEC.png"
			);
		}
	}
}

void Game::SetUnityBlendShape(const std::string& /*= ""*/)
{
	for (Mesh* mesh : *(s_instance->m_modelMeshSet))
	{
		if (mesh->m_name == "MTH_DEF")
		{
			s_instance->m_MTHBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			s_instance->m_MTHConstantBuffer = new ConstantBuffer(s_instance->m_renderer.m_rhiDevice, &(s_instance->m_MTHBlendInstance->m_buffer), 16U);
			s_instance->m_MTHWeightStructedBuffer = new StructuredBuffer(
					s_instance->m_renderer.m_rhiDevice, 
					s_instance->m_MTHBlendInstance->m_weights.data(), 
					sizeof(float), 
					s_instance->m_MTHBlendInstance->m_weights.size()
			);
			s_instance->m_MTHShapesStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_MTHBlendInstance->m_blendShape->GetBuffer(), 
				sizeof(BlendShapeParameter), 
				s_instance->m_MTHBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "EYE_DEF")
		{
			s_instance->m_EYEBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			s_instance->m_EYEConstantBuffer = new ConstantBuffer(s_instance->m_renderer.m_rhiDevice, &(s_instance->m_EYEBlendInstance->m_buffer), 16U);
			s_instance->m_EYEWeightStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_EYEBlendInstance->m_weights.data(), 
				sizeof(float), 
				s_instance->m_EYEBlendInstance->m_weights.size()
			);
			s_instance->m_EYEShapesStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_EYEBlendInstance->m_blendShape->GetBuffer(), 
				sizeof(BlendShapeParameter), 
				s_instance->m_EYEBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "BLW_DEF")
		{
			s_instance->m_BLWBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			s_instance->m_BLWConstantBuffer = new ConstantBuffer(s_instance->m_renderer.m_rhiDevice, &(s_instance->m_BLWBlendInstance->m_buffer), 16U);
			s_instance->m_BLWWeightStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_BLWBlendInstance->m_weights.data(), 
				sizeof(float), 
				s_instance->m_BLWBlendInstance->m_weights.size()
			);
			s_instance->m_BLWShapesStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_BLWBlendInstance->m_blendShape->GetBuffer(), 
				sizeof(BlendShapeParameter), 
				s_instance->m_BLWBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "EL_DEF")
		{
			s_instance->m_ELBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			s_instance->m_ELConstantBuffer = new ConstantBuffer(s_instance->m_renderer.m_rhiDevice, &(s_instance->m_ELBlendInstance->m_buffer), 16U);
			s_instance->m_ELWeightStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_ELBlendInstance->m_weights.data(), 
				sizeof(float), 
				s_instance->m_ELBlendInstance->m_weights.size()
			);
			s_instance->m_ELShapesStructedBuffer = new StructuredBuffer(
				s_instance->m_renderer.m_rhiDevice, 
				s_instance->m_ELBlendInstance->m_blendShape->GetBuffer(), 
				sizeof(BlendShapeParameter), 
				s_instance->m_ELBlendInstance->m_blendShape->GetBufferSize()
			);
		}
	}
}

void Game::SetUnityChanFacialExpression(const std::string& expressionName)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();
	s_instance->m_facialExpressNameForUnityChan = expressionName;
}

void Game::SetUnityChanFacialAnimation()
{
	float sample = 30.f;
	float duration = m_motion->m_duration;
	m_faceChangeEvent.AddEvent(.0000000000 * duration, SetUnityChanExpression_EyeClose);
	m_faceChangeEvent.AddEvent(.0606726483 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.0950656384 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.0995925739 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.1475780900 * duration, SetUnityChanExpression_Conf);
	m_faceChangeEvent.AddEvent(.1511996390 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.3126968150 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.3349536060 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.3910737930 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.4494034350 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.4547061320 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.4719398920 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.4816614990 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.5744587180 * duration, SetUnityChanExpression_Smile3);
	m_faceChangeEvent.AddEvent(.5815289020 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.7273530960 * duration, SetUnityChanExpression_EyeClose);
	m_faceChangeEvent.AddEvent(.7361909150 * duration, SetUnityChanExpression_Default);
	m_faceChangeEvent.AddEvent(.7817056780 * duration, SetUnityChanExpression_EyeClose);
	m_faceChangeEvent.AddEvent(.7923110720 * duration, SetUnityChanExpression_Default);
m_faceChangeEvent.AddEvent(1.f * duration, PlayScene);
}

void Game::SetUnityChanExpression_EyeClose(const std::string& /*= ""*/)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();

	s_instance->m_EYEBlendInstance->SetBlendWeight("EYE_DEF_C", 100);
	s_instance->m_BLWBlendInstance->SetBlendWeight("BLW_SMILE2", 100);
	s_instance->m_ELBlendInstance->SetBlendWeight("EYE_DEF_C", 100);
}

void Game::SetUnityChanExpression_Default(const std::string& /*= ""*/)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();
}

void Game::SetUnityChanExpression_Smile4(const std::string& /*= ""*/)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();

	s_instance->m_MTHBlendInstance->SetBlendWeight("MTH_SMILE1", 50);
	s_instance->m_EYEBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
	s_instance->m_BLWBlendInstance->SetBlendWeight("BLW_SMILE1", 100);
	s_instance->m_ELBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
}

void Game::SetUnityChanExpression_Smile3(const std::string& /*= ""*/)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();

	s_instance->m_EYEBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
	s_instance->m_BLWBlendInstance->SetBlendWeight("BLW_SMILE1", 100);
	s_instance->m_ELBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
}

void Game::SetUnityChanExpression_Conf(const std::string& /*= ""*/)
{
	s_instance->m_MTHBlendInstance->ResetBlendWeight();
	s_instance->m_EYEBlendInstance->ResetBlendWeight();
	s_instance->m_BLWBlendInstance->ResetBlendWeight();
	s_instance->m_ELBlendInstance->ResetBlendWeight();

	s_instance->m_MTHBlendInstance->SetBlendWeight("MTH_CONF", 100);
	s_instance->m_EYEBlendInstance->SetBlendWeight("EYE_CONF", 100);
	s_instance->m_BLWBlendInstance->SetBlendWeight("BLW_CONF", 100);
	s_instance->m_ELBlendInstance->SetBlendWeight("EYE_CONF", 100);
}

void Game::LoadMesh(const std::string& filePath)
{
	if (filePath.empty())
		return;
	DeleteMesh();
	std::pair<std::string,std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');
	bool result = false;
	if (fileAndPostFix.second == "fbx")
	{
		MeshBuilder mb;
		result = FBXUtils::LoadMesh(filePath, mb, s_instance->m_skeleton);
		if (result)
		{
			std::vector<Mesh*> *meshes = new std::vector<Mesh *>();
			s_instance->m_renderer.BuildMeshes(mb, *meshes);
			s_instance->m_modelMeshSet = meshes;
			//s_instance->m_modelMesh = s_instance->m_renderer.BuildMesh(mb);
		}
	}
	else //(fileAndPostFix.second == "bin")
	{
		s_instance->m_modelMesh = Mesh::GetOrLoad(filePath);
		if (s_instance->m_modelMesh)
		{
			result = true;
			s_instance->m_renderer.BuildMesh(*s_instance->m_modelMesh);
		}
	}

	if (result)
	{
		Console::Log("Load mesh \"" + filePath + "\" succeeded. ", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load mesh \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::SaveMesh(const std::string& filePath)
{
	if (Mesh::Save(filePath, s_instance->m_modelMesh))
		Console::Log("Mesh saved.", Rgba::GREEN);
	else
		Console::Log("Failed to save mesh.", Rgba::RED);
}

void Game::DeleteMesh(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	//SAFE_DELETE(s_instance->m_modelMesh);
	s_instance->m_modelMesh = nullptr;
}

void Game::LoadMeshes(const std::string& filePath)
{
	if (filePath.empty())
		return;
	DeleteMeshes();
	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');
	bool result = false;

	float loadMotionStart = GetCurrentTimeSeconds();
	float loadTimed = 0.f;

	if (fileAndPostFix.second == "fbx")
	{
		MeshBuilder mb;
		result = FBXUtils::LoadMesh(filePath, mb, s_instance->m_skeleton);
		if (result)
		{
			result = true;
			std::vector<Mesh*> *meshes = new std::vector<Mesh *>();
			s_instance->m_renderer.BuildMeshes(mb, *meshes);
			s_instance->m_modelMeshSet = meshes;
			//s_instance->m_modelMesh = s_instance->m_renderer.BuildMesh(mb);
		}
		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
	}
	else //(fileAndPostFix.second == "bin")
	{
		std::vector<Mesh*> *meshes = Mesh::GetOrLoadSet(filePath);
		if (meshes)
		{
			result = true;
			s_instance->m_renderer.BuildMeshes(*meshes);
			s_instance->m_modelMeshSet = meshes;
		}
		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
	}

	if (result)
	{
		Console::Log("Load mesh \"" + filePath + "\" succeeded. Took " + Time::ToString(loadTimed) +  " to Load.", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load mesh \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::SaveMeshes(const std::string& filePath)
{
	if (Mesh::SaveSet(filePath, s_instance->m_modelMeshSet))
		Console::Log("Mesh saved.", Rgba::GREEN);
	else
		Console::Log("Failed to save mesh.", Rgba::RED);
}

void Game::DeleteMeshes(const std::string& /*= ""*/)
{
	SAFE_DELETE(s_instance->m_modelMeshSet);
	s_instance->m_modelMeshSet = nullptr;
}

void Game::LoadSkeleton(const std::string& filePath)
{
	if (filePath.empty())
		return;
	DeleteSkeleton();
	
	bool result = false;
	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');
	
	if (fileAndPostFix.second == "fbx")
	{
		s_instance->m_skeleton = new Skeleton();
		result = FBXUtils::LoadSkeleton(s_instance->m_skeleton, filePath);
		if (!result)
			SAFE_DELETE(s_instance->m_skeleton);
	}
	else //(fileAndPostFix.second == "bin")
	{
		s_instance->m_skeleton = Skeleton::GetOrLoad(filePath);
		result = s_instance->m_skeleton ? true : false;
	}
	

	if (result)
	{
		Console::Log("Load skeleton \"" + filePath + "\" succeeded. " +
			std::to_string(s_instance->m_skeleton->m_jointMap.size()) + " bones.", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load skeleton \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::SaveSkeleton(const std::string& filePath)
{
	if (Skeleton::Save(filePath, s_instance->m_skeleton))
		Console::Log("Skeleton saved.", Rgba::GREEN);
	else
		Console::Log("Failed to save skeleton.", Rgba::RED);
}

void Game::DeleteSkeleton(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	//SAFE_DELETE(s_instance->m_skeleton);
	s_instance->m_skeleton = nullptr;
	SAFE_DELETE(s_instance->m_skeletonInstance);
}

void Game::LoadSkeletonMesh(const std::string& filePath)
{
	LoadSkeleton(filePath);
	LoadMesh(filePath);
}

void Game::LoadMotion(const std::string& filePath)
{
	if (filePath.empty())
		return;
	
	DeleteMotion();
	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');

	bool result = false;

	float loadMotionStart = GetCurrentTimeSeconds();
	float loadTimed = 0.f;

	if (fileAndPostFix.second == "fbx")
	{
		SAFE_DELETE(s_instance->m_motion);
		s_instance->m_motion = new Motion();

		result = FBXUtils::LoadTimedMotion(s_instance->m_motion, s_instance->m_skeleton, filePath.c_str());

		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;

		if (!result)
			SAFE_DELETE(s_instance->m_motion);
	}
	else //(fileAndPostFix.second == "bin")
	{
		s_instance->m_motion = Motion::GetOrLoad(filePath);
		result = s_instance->m_motion ? true : false;
		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
	}
	

	if (result)
	{
		
		//s_instance->m_motion->PreCalcSkinningMatrices(s_instance->m_skeleton);
		//s_instance->m_motion->PreCalcTimedSkinningMatrices(s_instance->m_skeleton);
		s_instance->m_skeletonInstance = new SkeletonInstance(s_instance->m_skeleton);
		s_instance->m_skeletonInstance->ApplyMotion(s_instance->m_motion, 0.f);
		s_instance->m_skeletonInstance->ApplyTimedMotion(s_instance->m_motion, 0.f);
		SAFE_DELETE(s_instance->m_poseBuffer);
		s_instance->m_poseBuffer = new StructuredBuffer(s_instance->m_renderer.m_rhiDevice, s_instance->m_skeletonInstance->m_currentPose.m_transforms.data(), sizeof(Matrix4), s_instance->m_skeletonInstance->m_currentPose.m_transforms.size());
		s_instance->m_renderer.SetStructuredBuffer(3, s_instance->m_poseBuffer);
		
		Console::Log("Load motion \"" + filePath + "\" succeeded. ", Rgba::GREEN);
		Console::Log("Sizeof time poses " + std::to_string(s_instance->m_motion->GetTimedPoseVolume()) + " bytes, took " + Time::ToString(loadTimed) + " to load.", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load motion \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::SaveMotion(const std::string& filePath)
{
	if (Motion::Save(filePath, s_instance->m_motion))
		Console::Log("Motion saved.", Rgba::GREEN);
	else
		Console::Log("Failed to save motion.", Rgba::RED);
}

void Game::DeleteMotion(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	//SAFE_DELETE(s_instance->m_motion);
	s_instance->m_motion = nullptr;
}

void Game::LoadHandMotion(const std::string& filePath)
{
	if (filePath.empty())
		return;

	DeleteHandMotion();
	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');

	bool result = false;

	float loadMotionStart = GetCurrentTimeSeconds();
	float loadTimed = 0.f;

	if (fileAndPostFix.second == "fbx")
	{
		SAFE_DELETE(s_instance->m_handMotion);
		s_instance->m_handMotion = new Motion();

		float loadMotionStart = GetCurrentTimeSeconds();
		//result = FBXUtils::LoadMotion(s_instance->m_motion, s_instance->m_skeleton, filePath.c_str(), 60);
		loadMotionStart = GetCurrentTimeSeconds() - loadMotionStart;

		float loadTimedMotionStart = GetCurrentTimeSeconds();
		result = FBXUtils::LoadTimedMotion(s_instance->m_handMotion, s_instance->m_skeleton, filePath.c_str());
		loadTimed = GetCurrentTimeSeconds() - loadTimedMotionStart;

		if (!result)
			SAFE_DELETE(s_instance->m_handMotion);
	}
	else //(fileAndPostFix.second == "bin")
	{
		s_instance->m_handMotion = Motion::GetOrLoad(filePath);
		result = s_instance->m_handMotion ? true : false;
		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
	}


	if (result)
	{
		
		//s_instance->m_motion->PreCalcSkinningMatrices(s_instance->m_skeleton);
		//s_instance->m_motion->PreCalcTimedSkinningMatrices(s_instance->m_skeleton);
		//s_instance->m_skeletonInstance = new SkeletonInstance(s_instance->m_skeleton);
		//s_instance->m_skeletonInstance->ApplyMotion(s_instance->m_motion, 0.f);
		//s_instance->m_skeletonInstance->ApplyTimedMotion(s_instance->m_motion, 0.f);
		//SAFE_DELETE(s_instance->m_poseBuffer);
		//s_instance->m_poseBuffer = new StructuredBuffer(s_instance->m_renderer.m_rhiDevice, s_instance->m_skeletonInstance->m_currentPose.m_transforms.data(), sizeof(Matrix4), s_instance->m_skeletonInstance->m_currentPose.m_transforms.size());
		//s_instance->m_renderer.SetStructuredBuffer(3, s_instance->m_poseBuffer);
		
		Console::Log("Load motion \"" + filePath + "\" succeeded. " +
			std::to_string(s_instance->m_handMotion->m_poses.size()) + " poses.", Rgba::GREEN);
		Console::Log("Sizeof time poses " + std::to_string(s_instance->m_handMotion->GetTimedPoseVolume()) + " bytes, took " + std::to_string(loadTimed) + " seconds to load.", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load motion \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::SaveHandMotion(const std::string& filePath)
{
	if (Motion::Save(filePath, s_instance->m_handMotion))
		Console::Log("Motion saved.", Rgba::GREEN);
	else
		Console::Log("Failed to save motion.", Rgba::RED);
}

void Game::DeleteHandMotion(const std::string& /*= ""*/)
{
	s_instance->m_handMotion = nullptr;
}

void Game::LoadFacialAnimation(const std::string& filePath)
{
	if (filePath.empty())
		return;

	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');

	bool result = false;

	float motionLoadTime = 0.f;
	float timedMotionLoadTime = 0.f;

	if (fileAndPostFix.second == "fbx")
	{
		SAFE_DELETE(s_instance->m_facialAnimations);
		s_instance->m_facialAnimations = new std::vector<BlendShapeAnimation *>();

		result = FBXUtils::LoadBlendShapeAnimation(s_instance->m_blendShapeAnimFilePath, *s_instance->m_facialAnimations, *s_instance->m_modelMeshSet);

		if (!result)
			SAFE_DELETE(s_instance->m_handMotion);
	}
	else //(fileAndPostFix.second == "bin")
	{
		//s_instance->m_handMotion = Motion::GetOrLoad(filePath);
		//result = s_instance->m_handMotion ? true : false;
	}


	if (result)
	{
		float loadMotionStart = GetCurrentTimeSeconds();
		//s_instance->m_motion->PreCalcSkinningMatrices(s_instance->m_skeleton);
		//s_instance->m_motion->PreCalcTimedSkinningMatrices(s_instance->m_skeleton);
		//s_instance->m_skeletonInstance = new SkeletonInstance(s_instance->m_skeleton);
		//s_instance->m_skeletonInstance->ApplyMotion(s_instance->m_motion, 0.f);
		//s_instance->m_skeletonInstance->ApplyTimedMotion(s_instance->m_motion, 0.f);
		//SAFE_DELETE(s_instance->m_poseBuffer);
		//s_instance->m_poseBuffer = new StructuredBuffer(s_instance->m_renderer.m_rhiDevice, s_instance->m_skeletonInstance->m_currentPose.m_transforms.data(), sizeof(Matrix4), s_instance->m_skeletonInstance->m_currentPose.m_transforms.size());
		//s_instance->m_renderer.SetStructuredBuffer(3, s_instance->m_poseBuffer);
		float loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
		Console::Log("Load facial motion \"" + filePath + "\" succeeded. ");
	}
	else
	{
		Console::Log("Load facial motion \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::LoadAnimation(const std::string& filePath)
{
	if (filePath.empty())
		return;

	DeleteAnimation();
	std::pair<std::string, std::string>fileAndPostFix = StringUtils::SplitAtLastToken(filePath, '.');

	bool result = false;

	float loadMotionStart = GetCurrentTimeSeconds();
	float loadTimed = 0.f;

	if (fileAndPostFix.second == "fbx")
	{
		SAFE_DELETE(s_instance->m_animation);
		s_instance->m_animation = new Animation();
		result = FBXUtils::LoadAnimation(filePath,*s_instance->m_animation);
		if (!result)
			SAFE_DELETE(s_instance->m_animation);
		loadTimed = GetCurrentTimeSeconds() - loadMotionStart;
	}
	else //(fileAndPostFix.second == "bin")
	{
		//s_instance->m_animation = Motion::GetOrLoad(filePath);
		//result = s_instance->m_motion ? true : false;
		result = false;
	}


	if (result)
	{
		Console::Log("Load animation \"" + filePath + "\" succeeded. Took " + Time::ToString(loadTimed) + " to load.", Rgba::GREEN);
	}
	else
	{
		Console::Log("Load animation \"" + filePath + "\" failed.", Rgba::RED);
	}
}

void Game::DeleteAnimation(const std::string& ignoredParm /*= ""*/)
{
	SAFE_DELETE(s_instance->m_animation);
}

void Game::ChangeTexture(const std::string& file)
{
	SAFE_DELETE(s_instance->m_texSample);
	s_instance->m_texSample = new Texture2D(s_instance->m_renderer.m_rhiDevice, "Data/Images/" + file);
}

void Game::ChangeShader(const std::string& shaderName)
{
	bool result = s_instance->m_renderer.SetShader(shaderName);
	if(result)
		s_instance->m_currentShaderName = shaderName;
}

void Game::S_Quit(const std::string& ignoredParm)
{
	UNUSED(ignoredParm);
	s_instance->Quit();
}

void Game::SetFov(const std::string& fov)
{
	s_instance->m_fov = stof(fov);
}

void Game::SetShowAxis(const std::string& isTrue)
{
	if (isTrue == "true")
		s_instance->m_doesDrawAxis = true;
	else
		s_instance->m_doesDrawAxis = false;
}

void Game::SetShowGrid(const std::string& isTrue)
{
	if (isTrue == "true")
		s_instance->m_doesDrawGrid = true;
	else
		s_instance->m_doesDrawGrid = false;
}

void Game::SetCameraTracking(const std::string& isTrue)
{
	s_instance->m_doesCameraTraceModel = StringUtils::ToBool(isTrue);
	if (s_instance->m_doesCameraTraceModel)
	{
		Vector3 socketPosition = s_instance->m_skeletonInstance->GetSocketTransform("Character1_Hips").GetPosition() * 0.01f;
		s_instance->m_modelPosition.position = socketPosition;

		Vector3 cameraTrackPosition = socketPosition + Vector3(0.f, 0.0f, 2.f);
		s_instance->m_camera.m_position = cameraTrackPosition;
	}
}

void Game::StopScene(const std::string& /*= ""*/)
{
	s_instance->m_audio.StopChannel(s_instance->m_bgmChannel);
	//s_instance->m_playTime = 2.f;
	s_instance->m_startPlaying = false;
}

void Game::PlayScene(const std::string& /*= ""*/)
{
	s_instance->m_bgmChannel = s_instance->m_audio.PlayAudio(s_instance->m_sound);
	s_instance->m_playTime = 2.f;
	s_instance->m_startPlaying = true;
	s_instance->m_faceChangeEvent.Reset();
	s_instance->m_faceChangeEvent.Update(s_instance->m_playTime);
}

void Game::InitRendering()
{
	int width = g_windowWidth;
	int height = g_windowHeight;

	Config::GetInt(width, "window_width");
	Config::GetInt(height, "window_height");

	m_renderer.Setup(width, height);
	m_renderer.SetViewport(0, 0, width, height);

	m_currentShaderName = "NoEffect";
	Config::GetString(m_currentShaderName, "shader");
	m_renderer.SetShader(m_currentShaderName);

	
}

void Game::BuildVbo()
{
	Vertex3 vertices[] = {
		Vertex3(Vector3(-0.5f, -0.5f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.f,0.f,-1.f)),
		Vertex3(Vector3(0.5f, -0.5f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.f,0.f,-1.f)),
		Vertex3(Vector3(0.5f, 0.5f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.f,0.f,-1.f)),
		Vertex3(Vector3(-0.5f, 0.5f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.f,0.f,-1.f)),
	};

	AABB3 bound(Vector3(-1.f, 0.f, -1.f), Vector3(1.f, 2.f, 1.f));

	//std::vector<Vertex3> cubeVertices = MeshBuilder::Cube(bound);

	Vertex3 axis[] = {
		Vertex3(Vector3(0.f,0.f,0.f),Rgba::RED, Vector2(1.0f, 1.0f), Vector3(0.f,0.f,0.f)),
		Vertex3(Vector3(10.f,0.f,0.f),Rgba::RED, Vector2(0.0f, 1.0f), Vector3(0.f,0.f,0.f)),

		Vertex3(Vector3(0.f,0.f,0.f),Rgba::GREEN, Vector2(1.0f, 1.0f), Vector3(0.f,0.f,0.f)),
		Vertex3(Vector3(0.f,10.f,0.f),Rgba::GREEN, Vector2(0.0f, 1.0f), Vector3(0.f,0.f,0.f)),

		Vertex3(Vector3(0.f,0.f,0.f),Rgba::BLUE, Vector2(1.0f, 1.0f), Vector3(0.f,0.f,0.f)),
		Vertex3(Vector3(0.f,0.f,10.f),Rgba::BLUE, Vector2(0.0f, 1.0f), Vector3(0.f,0.f,0.f)),
	};

	std::vector<Vertex3> grids;
	Rgba gridLineColor = Rgba(180, 180, 180);

/*
	for (float i = -10.f; i < 10.f; i += 1.f)
	{
		for (float j = -10.f; j < 10.f; j += 1.f)
		{
			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j+1.f, 0.f, i), gridLineColor));

			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j, 0.f, i + 1.f), gridLineColor));
		}
	}*/
	gridLineColor.ScaleRGB(0.5f);
	float gridLength = 5.f;
	float gridGap = 0.1f;
	grids.reserve((gridLength * 2.f / gridGap) * (gridLength * 2.f / gridGap) * 4.f);
	for (float i = -gridLength; i < gridLength; i += gridGap)
	{
		for (float j = -gridLength; j < gridLength; j += gridGap)
		{
			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j + gridGap, 0.f, i), gridLineColor));

			grids.push_back(Vertex3(Vector3(j, 0.f, i), gridLineColor));
			grids.push_back(Vertex3(Vector3(j, 0.f, i + gridGap), gridLineColor));
		}
		
	}

	std::vector<Vertex3> cylinderVertices = MeshBuilder::Cylinder(Vector3(3.5f, 0.f, 0.5f), 3.1415f, 0.5f, 64);
	std::vector<Vertex3> sphereVertices = MeshBuilder::Sphere(Vector3(), 10, 64);

	quadVbo = m_renderer.CreateVertexBuffer(vertices, sizeof(vertices) / sizeof(Vertex3), PRIMITIVE_QUADS, false);
	//cubeVbo = m_renderer.CreateVertexBuffer(cubeVertices.data(), cubeVertices.size(), PRIMITIVE_QUADS);
	axisVbo = m_renderer.CreateVertexBuffer(axis, sizeof(axis) / sizeof(Vertex3), PRIMITIVE_LINES, false, false);
	gridVbo = m_renderer.CreateVertexBuffer(grids.data(), grids.size(), PRIMITIVE_LINES);
	cylinderVbo = m_renderer.CreateVertexBuffer(cylinderVertices.data(), cylinderVertices.size(), PRIMITIVE_QUADS, false, false);
	//cylinderTbnLines = m_renderer.CreateTbnVertexBuffer(cylinderVertices.data(), cylinderVertices.size(), PRIMITIVE_QUADS);
	sphereVbo = m_renderer.CreateVertexBuffer(sphereVertices.data(), sphereVertices.size(), PRIMITIVE_QUADS, false, false);
	//sphereTbnLines = m_renderer.CreateTbnVertexBuffer(sphereVertices.data(), sphereVertices.size(), PRIMITIVE_QUADS);
}

void Game::CleanupRendering()
{
	m_renderer.Destroy();

// 	SAFE_DELETE(quadVbo);
// 	SAFE_DELETE(cubeVbo);
// 	SAFE_DELETE(axisVbo);
// 	SAFE_DELETE(gridVbo);
}

void Game::LoadMeshAndMotion()
{
	LoadSkeleton(m_skeletonFilePath);
	LoadMeshes(m_meshFilePath);
	LoadMotion(m_motionPath);
	LoadHandMotion(m_handMotionPath);
	LoadFacialAnimation(m_blendShapeAnimFilePath);
	LoadAnimation(m_animationFilePath);
}
	
void Game::LoadMeshAndMotionStatic()
{
	s_instance->LoadMeshAndMotion();
}

void Game::SetupInput()
{
	/*InputDeviceMouse *mouse = GetMouse();
	InputDeviceKeyboard *keyboard = GetKeyboard();
	// InputDeviceGamepad *gamepad = GetGamepad();

	// Some other mouse things
	mouse->contain(true);
	mouse->show_system_cursor(false);

	// Setup Quit Key
	VirtualInputValue *quit = m_controller.get_input_value("quit");
	quit->watch(keyboard->get_value(KB_ESCAPE));

	// Directions
	InputAxis *forward = m_controller.get_input_axis("forward");
	forward->watch(keyboard->get_value(KB_W), keyboard->get_value(KB_S));

	InputAxis *right = m_controller.get_input_axis("right");
	right->watch(keyboard->get_value(KB_D), keyboard->get_value(KB_A));

	InputAxis *up = m_controller.get_input_axis("up");
	up->watch(keyboard->get_value(KB_SPACEBAR), keyboard->get_value(KB_SHIFTLEFT));

	// Rotation
	InputAxis *yaw = m_controller.get_input_axis("yaw");
	yaw->watch(&mouse->rel_x);

	InputAxis *pitch = m_controller.get_input_axis("pitch");
	pitch->watch(&mouse->rel_y);*/
}
