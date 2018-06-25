#pragma once
#include <string>
#include "Engine\Renderer\BlendShape.hpp"
#include "Engine\RHI\ConstantBuffer.hpp"
#include "Engine\RHI\StructuredBuffer.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include <vector>
#include "Engine\Renderer\Skeleton.hpp"
#include "Engine\Renderer\Animation.hpp"
#include "SpringJoint.hpp"
#include "Engine\Core\EventTimeLine.hpp"
#include "Engine\Renderer\ParticleEffect.hpp"
#include "Engine\Audio\Audio.hpp"
#include "Engine\Renderer\CameraZXY.hpp"
#include "Engine\Renderer\Light.hpp"

struct model_position_t
{
	Vector3 position;
	float padding;
};

class UnitychanDancingStage
{
public:
	bool m_startPlaying = false;
	float m_playTime = 0;
	bool m_doesCameraTraceModel = true;

	SoundID m_sound;
	AudioChannelHandle m_bgmChannel;
	CameraZXY m_camera;

	Light m_frontLight;
	Light m_backLight;

	Mesh *m_modelMesh = nullptr;
	std::vector<Mesh*> *m_modelMeshSet = nullptr;
	std::vector<BlendShapeAnimation*> *m_facialAnimations = nullptr;
	Skeleton* m_skeleton = nullptr;
	SkeletonInstance* m_skeletonInstance = nullptr;
	Animation* m_animation = nullptr;
	std::string m_meshFilePath = "";
	std::string m_skeletonFilePath = "";
	std::string m_animationFilePath = "";
	std::string m_blendShapeAnimFilePath = "";
	StructuredBuffer* m_poseBuffer = nullptr;

	std::vector<SpringJoint> m_springs;

	EventTimeLine m_faceChangeEvent;

	std::string m_facialExpressNameForUnityChan = "";

	std::string m_motionPath = "";
	Motion* m_motion = nullptr;
	std::string m_handMotionPath = "";
	Motion* m_handMotion = nullptr;

	model_position_t m_modelPosition;
	ConstantBuffer* m_modelPositionBuffer = nullptr;
	std::vector<float> m_spectrum;
	StructuredBuffer* m_spectrumBuffer;

	ParticleEffect* m_leftHandRibbonEffect = nullptr;
	ParticleEffect* m_rightHandRibbonEffect = nullptr;

	ParticleEffect* m_leftHandParticleEffect = nullptr;
	ParticleEffect* m_rightHandParticleEffect = nullptr;

	BlendShapeInstance	*m_MTHBlendInstance = nullptr;
	ConstantBuffer		*m_MTHConstantBuffer = nullptr;
	StructuredBuffer	*m_MTHWeightStructedBuffer = nullptr;
	StructuredBuffer	*m_MTHShapesStructedBuffer = nullptr;

	BlendShapeInstance	*m_BLWBlendInstance = nullptr;
	ConstantBuffer		*m_BLWConstantBuffer = nullptr;
	StructuredBuffer	*m_BLWWeightStructedBuffer = nullptr;
	StructuredBuffer	*m_BLWShapesStructedBuffer = nullptr;

	BlendShapeInstance	*m_EYEBlendInstance = nullptr;
	ConstantBuffer		*m_EYEConstantBuffer = nullptr;
	StructuredBuffer	*m_EYEWeightStructedBuffer = nullptr;
	StructuredBuffer	*m_EYEShapesStructedBuffer = nullptr;

	BlendShapeInstance	*m_ELBlendInstance = nullptr;
	ConstantBuffer		*m_ELConstantBuffer = nullptr;
	StructuredBuffer	*m_ELWeightStructedBuffer = nullptr;
	StructuredBuffer	*m_ELShapesStructedBuffer = nullptr;

	VertexBuffer *m_gridVbo = nullptr;
	
public:

	void Start();
	void Update();
	void Render() const;

	void UpdateSkeleton();
	void UpdateSpringJoints();
	void UpdateUnitychanBlendShape();
	void UpdateUnitychanLipSync();
	void RenderUnitychan() const;
	void SetUnityChanSpringJoints();
	void SetUnityChanMaterial();
	void SetUnityBlendShape();
	void SetUnityChanFacialExpression(const std::string& expressionName);
	void SetUnityChanFacialAnimation();
	void SetUnityChanExpression_EyeClose();
	void SetUnityChanExpression_Default();
	void SetUnityChanExpression_Smile4();
	void SetUnityChanExpression_Smile3();
	void SetUnityChanExpression_Conf();

	void LoadMeshAndMotion();
	void BuildVbo();

	void PlayScene();
	void StopScene();
};