#include "UnitychanDancingStage.hpp"
#include "Game.hpp"
#include "Engine\Core\Event.hpp"

void UnitychanDancingStage::Start()
{

	auto& renderer = Game::GetInstance()->m_renderer;

	m_camera.m_position.y = 1.2f;
	m_camera.m_position.z = 4.f;
	m_camera.m_rotation.x = 10.f;
	m_camera.m_rotation.y = 180.f;

	m_meshFilePath = "Data/bin/unitychan.mesh";
	m_skeletonFilePath = "Data/bin/unitychan.skel";
	m_motionPath = "Data/bin/unit_in_the_sky.anim";
	m_handMotionPath = "Data/bin/unit_in_the_sky_hand.anim";
	m_animationFilePath = "Data/animation/LipSync.fbx";
	m_blendShapeAnimFilePath = "";

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

	m_spectrum.resize(1024, 0.f);
	m_spectrumBuffer = new StructuredBuffer(renderer.m_rhiDevice, m_spectrum.data(), sizeof(float), m_spectrum.size());
	renderer.SetStructuredBuffer(10, m_spectrumBuffer);
	m_modelPosition.position = Vector3();
	m_modelPositionBuffer = new ConstantBuffer(renderer.m_rhiDevice, &m_modelPosition, sizeof(model_position_t));
	renderer.SetConstantBuffer(10, m_modelPositionBuffer);

	m_doesCameraTraceModel = true;

	m_startPlaying = false;
	m_playTime = 0.f;

	m_frontLight.SetPosition(Vector3(-2.5, 0, -2.5));
	m_frontLight.SetColor(Rgba::WHITE);
	m_frontLight.m_attributes.ATTENUATION = Vector4(1.f, 0.f, 0.f, 0.f);
	m_frontLight.m_attributes.SPEC_ATTENUATION = Vector4(16.f, 16.f, 16.f, 0.f);

	m_backLight.SetPosition(Vector3(2.5, 0, 2.5));
	m_backLight.SetColor(Rgba::BLACK);
	m_backLight.m_attributes.ATTENUATION = Vector4(1.f, 0.f, 0.f, 0.f);
	m_backLight.m_attributes.SPEC_ATTENUATION = Vector4(16.f, 16.f, 16.f, 0.f);

	LoadMeshAndMotion();
	SetUnityChanMaterial();
	//SetUnityChanCandyMaterial();
	SetUnityBlendShape();
	SetUnityChanFacialAnimation();
	SetUnityChanSpringJoints();
	BuildVbo();

	m_sound = Game::GetInstance()->m_audio.CreateOrGetSound("Data/audio/Unite In The Sky (full).mp3");

	PlayScene();
}

void UnitychanDancingStage::Update()
{
	m_camera.Update();

	UpdateSkeleton();
	UpdateUnitychanBlendShape();
	Game::GetInstance()->m_audio.Update();

	if (m_startPlaying)
		ParticleSystem::Update();

	if (m_bgmChannel)
	{
		Game::GetInstance()->m_audio.GetSpectrum(m_bgmChannel, m_spectrum);
		m_spectrumBuffer->Update(Game::GetInstance()->m_renderer.m_rhiContext, m_spectrum.data());
	}
}

void UnitychanDancingStage::Render() const
{
	auto& renderer = Game::GetInstance()->m_renderer;

	const IntVector2& windowSize = renderer.GetWindowSize();
	float aspectRatio = (float)windowSize.x / (float)windowSize.y;

	renderer.SetCamera(m_camera);
	/*renderer.SetEyePosition(m_camera.m_position);
	renderer.SetViewMatrix(m_camera.GetViewMatrix());
	renderer.SetPerspectiveProjection(m_camera.m_fov, aspectRatio, 0.1f, 50.0f);*/

	renderer.EnablePointLight(1, m_frontLight);
	renderer.EnablePointLight(2, m_backLight);

	if (m_skeleton || m_modelMesh || m_skeletonInstance || m_motion) {
		renderer.PushMatrix();
		renderer.Scale(0.01f);
		if (m_motion || m_animation)
			renderer.SetShader("Pose");
		if (m_modelMeshSet)
			RenderUnitychan();

		renderer.SetShader(nullptr);
		renderer.PopMatrix();
	}
	
	if (m_bgmChannel)
		renderer.SetShader("MusicFloor");
	renderer.DrawVbo(PRIMITIVE_LINES, m_gridVbo);

	renderer.DrawParticleSystem(m_camera.m_position);
}

void UnitychanDancingStage::UpdateSkeleton()
{
	if (m_skeletonInstance && (m_motion))
	{
		//static float time = 0.f;
		float& time = m_playTime;
		if (m_startPlaying)
			time += Time::deltaSeconds;

		m_skeletonInstance->ApplyTimedMotionAdditive(m_motion, time);
		if (m_handMotion)
			m_skeletonInstance->ApplyTimedMotionAdditive(m_handMotion, time);

		UpdateSpringJoints();

		m_skeletonInstance->EvaluateSkinning();

		std::vector<Matrix4>& skinningTransforms = m_skeletonInstance->m_currentSkinning.m_transforms;
		m_poseBuffer->Update(Game::GetInstance()->m_renderer.m_rhiContext, skinningTransforms.data());
		Vector3 socketPosition = m_skeletonInstance->GetSocketTransform("Character1_Hips").GetPosition() * 0.01f;
		m_modelPosition.position = socketPosition;
		m_modelPositionBuffer->Update(Game::GetInstance()->m_renderer.m_rhiContext, &m_modelPosition);
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

void UnitychanDancingStage::UpdateSpringJoints()
{
	for (SpringJoint& joint : m_springs)
		joint.Update();
}

void UnitychanDancingStage::UpdateUnitychanBlendShape()
{
	auto& renderer = Game::GetInstance()->m_renderer;
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



		if (m_animation)
			UpdateUnitychanLipSync();

		m_MTHWeightStructedBuffer->Update(renderer.m_rhiContext, m_MTHBlendInstance->m_weights.data());
		m_EYEWeightStructedBuffer->Update(renderer.m_rhiContext, m_EYEBlendInstance->m_weights.data());
		m_BLWWeightStructedBuffer->Update(renderer.m_rhiContext, m_BLWBlendInstance->m_weights.data());
		m_ELWeightStructedBuffer->Update(renderer.m_rhiContext, m_ELBlendInstance->m_weights.data());
	}
}

void UnitychanDancingStage::UpdateUnitychanLipSync()
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

void UnitychanDancingStage::RenderUnitychan() const
{
	auto& renderer = Game::GetInstance()->m_renderer;
	for (Mesh* mesh : *m_modelMeshSet)
	{
		if (mesh->m_name == "MTH_DEF")
		{
			renderer.SetStructuredBuffer(4, m_MTHShapesStructedBuffer);
			renderer.SetStructuredBuffer(5, m_MTHWeightStructedBuffer);
			renderer.SetConstantBuffer(5, m_MTHConstantBuffer);
		}
		else if (mesh->m_name == "EYE_DEF")
		{
			renderer.SetStructuredBuffer(4, m_EYEShapesStructedBuffer);
			renderer.SetStructuredBuffer(5, m_EYEWeightStructedBuffer);
			renderer.SetConstantBuffer(5, m_EYEConstantBuffer);
		}
		else if (mesh->m_name == "BLW_DEF")
		{
			renderer.SetStructuredBuffer(4, m_BLWShapesStructedBuffer);
			renderer.SetStructuredBuffer(5, m_BLWWeightStructedBuffer);
			renderer.SetConstantBuffer(5, m_BLWConstantBuffer);
		}
		else if (mesh->m_name == "EL_DEF")
		{
			renderer.SetStructuredBuffer(4, m_ELShapesStructedBuffer);
			renderer.SetStructuredBuffer(5, m_ELWeightStructedBuffer);
			renderer.SetConstantBuffer(5, m_ELConstantBuffer);
		}
		else
		{
			renderer.SetStructuredBuffer(4, nullptr);
			renderer.SetStructuredBuffer(5, nullptr);
			renderer.SetConstantBuffer(5, nullptr);
		}
		renderer.SetMaterial(mesh->m_material);
		renderer.DrawMesh(mesh);
	}
}

void UnitychanDancingStage::SetUnityChanSpringJoints()
{
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

}

void UnitychanDancingStage::SetUnityChanMaterial()
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

void UnitychanDancingStage::SetUnityBlendShape()
{
	auto& renderer = Game::GetInstance()->m_renderer;
	for (Mesh* mesh : *(m_modelMeshSet))
	{
		if (mesh->m_name == "MTH_DEF")
		{
			m_MTHBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			m_MTHConstantBuffer = new ConstantBuffer(renderer.m_rhiDevice, &(m_MTHBlendInstance->m_buffer), 16U);
			m_MTHWeightStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_MTHBlendInstance->m_weights.data(),
				sizeof(float),
				m_MTHBlendInstance->m_weights.size()
			);
			m_MTHShapesStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_MTHBlendInstance->m_blendShape->GetBuffer(),
				sizeof(BlendShapeParameter),
				m_MTHBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "EYE_DEF")
		{
			m_EYEBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			m_EYEConstantBuffer = new ConstantBuffer(renderer.m_rhiDevice, &(m_EYEBlendInstance->m_buffer), 16U);
			m_EYEWeightStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_EYEBlendInstance->m_weights.data(),
				sizeof(float),
				m_EYEBlendInstance->m_weights.size()
			);
			m_EYEShapesStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_EYEBlendInstance->m_blendShape->GetBuffer(),
				sizeof(BlendShapeParameter),
				m_EYEBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "BLW_DEF")
		{
			m_BLWBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			m_BLWConstantBuffer = new ConstantBuffer(renderer.m_rhiDevice, &(m_BLWBlendInstance->m_buffer), 16U);
			m_BLWWeightStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_BLWBlendInstance->m_weights.data(),
				sizeof(float),
				m_BLWBlendInstance->m_weights.size()
			);
			m_BLWShapesStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_BLWBlendInstance->m_blendShape->GetBuffer(),
				sizeof(BlendShapeParameter),
				m_BLWBlendInstance->m_blendShape->GetBufferSize()
			);
		}
		else if (mesh->m_name == "EL_DEF")
		{
			m_ELBlendInstance = new BlendShapeInstance(&mesh->m_blendShape);
			m_ELConstantBuffer = new ConstantBuffer(renderer.m_rhiDevice, &(m_ELBlendInstance->m_buffer), 16U);
			m_ELWeightStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_ELBlendInstance->m_weights.data(),
				sizeof(float),
				m_ELBlendInstance->m_weights.size()
			);
			m_ELShapesStructedBuffer = new StructuredBuffer(
				renderer.m_rhiDevice,
				m_ELBlendInstance->m_blendShape->GetBuffer(),
				sizeof(BlendShapeParameter),
				m_ELBlendInstance->m_blendShape->GetBufferSize()
			);
		}
	}
}

void UnitychanDancingStage::SetUnityChanFacialExpression(const std::string& expressionName)
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();
	m_facialExpressNameForUnityChan = expressionName;
}

void UnitychanDancingStage::SetUnityChanFacialAnimation()
{
	float sample = 30.f;
	float duration = m_motion->m_duration;
	m_faceChangeEvent.AddEvent(.0000000000f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_EyeClose));
	m_faceChangeEvent.AddEvent(.0606726483f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.0950656384f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.0995925739f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.1475780900f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Conf));
	m_faceChangeEvent.AddEvent(.1511996390f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.3126968150f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.3349536060f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.3910737930f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.4494034350f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.4547061320f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.4719398920f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.4816614990f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.5744587180f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Smile3));
	m_faceChangeEvent.AddEvent(.5815289020f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.7273530960f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_EyeClose));
	m_faceChangeEvent.AddEvent(.7361909150f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(.7817056780f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_EyeClose));
	m_faceChangeEvent.AddEvent(.7923110720f * duration, Event<>(this, &UnitychanDancingStage::SetUnityChanExpression_Default));
	m_faceChangeEvent.AddEvent(1.f * duration, Event<>(this, &UnitychanDancingStage::PlayScene));
}

void UnitychanDancingStage::SetUnityChanExpression_EyeClose()
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();

	m_EYEBlendInstance->SetBlendWeight("EYE_DEF_C", 100);
	m_BLWBlendInstance->SetBlendWeight("BLW_SMILE2", 100);
	m_ELBlendInstance->SetBlendWeight("EYE_DEF_C", 100);
}

void UnitychanDancingStage::SetUnityChanExpression_Default()
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();
}

void UnitychanDancingStage::SetUnityChanExpression_Smile4()
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();

	m_MTHBlendInstance->SetBlendWeight("MTH_SMILE1", 50);
	m_EYEBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
	m_BLWBlendInstance->SetBlendWeight("BLW_SMILE1", 100);
	m_ELBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
}

void UnitychanDancingStage::SetUnityChanExpression_Smile3()
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();

	m_EYEBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
	m_BLWBlendInstance->SetBlendWeight("BLW_SMILE1", 100);
	m_ELBlendInstance->SetBlendWeight("EYE_SMILE1", 100);
}

void UnitychanDancingStage::SetUnityChanExpression_Conf()
{
	m_MTHBlendInstance->ResetBlendWeight();
	m_EYEBlendInstance->ResetBlendWeight();
	m_BLWBlendInstance->ResetBlendWeight();
	m_ELBlendInstance->ResetBlendWeight();

	m_MTHBlendInstance->SetBlendWeight("MTH_CONF", 100);
	m_EYEBlendInstance->SetBlendWeight("EYE_CONF", 100);
	m_BLWBlendInstance->SetBlendWeight("BLW_CONF", 100);
	m_ELBlendInstance->SetBlendWeight("EYE_CONF", 100);
}

void UnitychanDancingStage::LoadMeshAndMotion()
{
	m_skeleton = Skeleton::GetOrLoad(m_skeletonFilePath);
	m_modelMeshSet = Mesh::GetOrLoadSet(m_meshFilePath);
	m_motion = Motion::GetOrLoad(m_motionPath);
	m_handMotion = Motion::GetOrLoad(m_handMotionPath);
	//m_facialAnimations = BlendShapeAnimation::LoadFromFBX(m_blendShapeAnimFilePath, m_modelMeshSet);
	m_animation = Animation::LoadFromFBX(m_animationFilePath);

	auto& renderer = Game::GetInstance()->m_renderer;

	m_skeletonInstance = new SkeletonInstance(m_skeleton);
	m_skeletonInstance->ApplyMotion(m_motion, 0.f);
	m_skeletonInstance->ApplyTimedMotion(m_motion, 0.f);
	SAFE_DELETE(m_poseBuffer);
	m_poseBuffer = new StructuredBuffer(renderer.m_rhiDevice, m_skeletonInstance->m_currentPose.m_transforms.data(), sizeof(Matrix4), m_skeletonInstance->m_currentPose.m_transforms.size());
	renderer.SetStructuredBuffer(3, m_poseBuffer);
}

void UnitychanDancingStage::BuildVbo()
{
	std::vector<Vertex3> grids;
	Rgba gridLineColor = Rgba(180, 180, 180);

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
	m_gridVbo = Game::GetInstance()->m_renderer.CreateVertexBuffer(grids.data(), grids.size(), PRIMITIVE_LINES);
}

void UnitychanDancingStage::PlayScene()
{
	m_bgmChannel = Game::GetInstance()->m_audio.PlayAudio(m_sound);
	m_playTime = 2.f;
	m_startPlaying = true;
	m_faceChangeEvent.Reset();
	m_faceChangeEvent.Update(m_playTime);
}

void UnitychanDancingStage::StopScene()
{
	Game::GetInstance()->m_audio.StopChannel(m_bgmChannel);
	//s_instance->m_playTime = 2.f;
	m_startPlaying = false;
}
