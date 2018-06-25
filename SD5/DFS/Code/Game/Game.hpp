#pragma once

#include "Engine/RHI/RHI.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/D3D11Renderer.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Renderer/CameraZXY.hpp"
#include "Engine/Tools/fbx.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/Motion.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "SpringJoint.hpp"
#include "Scene.hpp"
#include "Engine/Core/EventTimeLine.hpp"

enum eQuitReason
{
	QUITREASON_NONE,
	QUITREASON_USER,
};

struct time_constant_t
{
	float time;

	float screenWidth;

	float screenHeight;

	float shaderParameter;
};

struct model_position_t
{
	Vector3 position;
	float padding;
};


class Game
{
public:
	Game();
	~Game();
	void Run();
	void Start();
	void AssignConsoleCommands();
	void RunFrame();
	void End();

	void Update(float deltaSeconds);
	void UpdateSkeleton(float deltaSeconds);
	

	void Render();
	void SetAndDrawLights();

	void Quit(eQuitReason reason = QUITREASON_USER);
	bool IsQuitting() const;
	bool IsRunning() const;
	void CheckInput(float deltaSeconds);

	static void ToggleFullScreen(const std::string& = "");
	static void ToggleFullScreenBorderless(const std::string& = "");

	void UpdateUnitychanBlendShape();
	void UpdateUnitychanLipSync();
	void RenderUnitychan();
	void SetUnityChanSpringJoints();
	void SetUnityChanFacialAnimation();
	static void SetUnityChanMaterial(const std::string& = "");
	static void SetUnityChanCandyMaterial(const std::string& = "");
	static void SetUnityBlendShape(const std::string& = "");
	static void SetUnityChanFacialExpression(const std::string& expressionName);
	static void SetUnityChanExpression_EyeClose(const std::string& = "");
	static void SetUnityChanExpression_Default(const std::string& = "");
	static void SetUnityChanExpression_Smile4(const std::string& = "");
	static void SetUnityChanExpression_Smile3(const std::string& = "");
	static void SetUnityChanExpression_Conf(const std::string& = "");


	static void LoadMesh(const std::string& filePath);
	static void SaveMesh(const std::string& filePath);
	static void DeleteMesh(const std::string& = "");

	static void LoadMeshes(const std::string& filePath);
	static void SaveMeshes(const std::string& filePath);
	static void DeleteMeshes(const std::string& = "");

	static void LoadSkeleton(const std::string& filePath);
	static void SaveSkeleton(const std::string& filePath);
	static void DeleteSkeleton(const std::string& = "");

	static void LoadSkeletonMesh(const std::string& filePath);

	static void LoadMotion(const std::string& filePath);
	static void SaveMotion(const std::string& filePath);
	static void DeleteMotion(const std::string& = "");

	static void LoadHandMotion(const std::string& filePath);
	static void SaveHandMotion(const std::string& filePath);
	static void DeleteHandMotion(const std::string& = "");

	static void LoadFacialAnimation(const std::string& filePath);
	static void SaveFacialAnimation(const std::string& filePath);

	static void LoadAnimation(const std::string& filePath);
	static void SaveAnimation(const std::string& filePath);
	static void DeleteAnimation(const std::string& = "");

	static void ChangeTexture(const std::string& file);
	static void ChangeShader(const std::string& shaderName);
	static void S_Quit(const std::string& = "");
	static void SetFov(const std::string& fov);
	static void SetShowAxis(const std::string& isTrue);
	static void SetShowGrid(const std::string& isTrue);

	static void SetCameraTracking(const std::string& isTrue);

	static void StopScene(const std::string& = "");
	static void PlayScene(const std::string& = "");

public:
	static Game* GetInstance() { return s_instance; }
	static D3D11Renderer* GetRenderer() { return &s_instance->m_renderer; }
public:
	void InitRendering();
	void BuildVbo();
	void CleanupRendering();

	void LoadMeshAndMotion();

	static void LoadMeshAndMotionStatic();

	//------------------------------------------------------------------------
	// Not sure if you will have a system like this by SD3
	// but leaving it here as a potential bonus topic for AES or SD5
	void SetupInput();

public:
	D3D11Renderer m_renderer;
	AudioSystem m_audio;
	ShaderProgram *m_shader;

	bool m_usePhysxForSpringJoins = false;

	Scene m_scene;

	SoundID m_sound;
	AudioChannelHandle m_bgmChannel;

	Mesh *m_modelMesh;
	std::vector<Mesh*> *m_modelMeshSet;
	std::vector<BlendShapeAnimation*> *m_facialAnimations;
	Skeleton* m_skeleton;
	SkeletonInstance* m_skeletonInstance;
	Animation* m_animation;
	std::string m_meshFilePath;
	std::string m_skeletonFilePath;
	std::string m_animationFilePath;
	std::string m_blendShapeAnimFilePath;
	StructuredBuffer* m_poseBuffer;

	std::vector<SpringJoint> m_springs;

	EventTimeLine m_faceChangeEvent;

	std::string m_facialExpressNameForUnityChan;

	std::string m_motionPath;
	Motion* m_motion;
	std::string m_handMotionPath;
	Motion* m_handMotion;

	model_position_t m_modelPosition;
	ConstantBuffer* m_modelPositionBuffer;
	std::vector<float> m_spectrum;
	StructuredBuffer* m_spectrumBuffer;

	ParticleEffect* m_leftHandRibbonEffect;
	ParticleEffect* m_rightHandRibbonEffect;

	ParticleEffect* m_leftHandParticleEffect;
	ParticleEffect* m_rightHandParticleEffect;

	BlendShapeInstance	*m_MTHBlendInstance;
	ConstantBuffer		*m_MTHConstantBuffer;
	StructuredBuffer	*m_MTHWeightStructedBuffer;
	StructuredBuffer	*m_MTHShapesStructedBuffer;

	BlendShapeInstance	*m_BLWBlendInstance;
	ConstantBuffer		*m_BLWConstantBuffer;
	StructuredBuffer	*m_BLWWeightStructedBuffer;
	StructuredBuffer	*m_BLWShapesStructedBuffer;

	BlendShapeInstance	*m_EYEBlendInstance;
	ConstantBuffer		*m_EYEConstantBuffer;
	StructuredBuffer	*m_EYEWeightStructedBuffer;
	StructuredBuffer	*m_EYEShapesStructedBuffer;

	BlendShapeInstance	*m_ELBlendInstance;
	ConstantBuffer		*m_ELConstantBuffer;
	StructuredBuffer	*m_ELWeightStructedBuffer;
	StructuredBuffer	*m_ELShapesStructedBuffer;

	VertexBuffer *quadVbo;
	VertexBuffer *cubeVbo;
	VertexBuffer *axisVbo;
	VertexBuffer *gridVbo;
	VertexBuffer *cylinderVbo;
	VertexBuffer *cylinderTbnLines;
	VertexBuffer *sphereVbo;
	VertexBuffer *sphereTbnLines;

	bool m_doesCameraTraceModel;

	bool m_doesDrawAxis;
	bool m_doesDrawGrid;

	bool m_startPlaying;
	float m_playTime;

	// Personal Class - Potential Bonus Lecture
	//InputSystem m_controller;

	eQuitReason m_quitReason;

	Texture2D* m_texSample;
	Texture2D* m_texNormal;
	Texture2D* m_texSpec;

	Texture2D* m_texSample2;

	float m_timeOfLastRunframe;

	std::string m_currentShaderName;

	time_constant_t time;
	ConstantBuffer *time_constants;


	MatrixConstantBuffer mats;
	ConstantBuffer *matConstants;

	Vector3 m_centerOfLights;
	float m_currentRotationOfLights;
	float m_radiusOfLights;
	float m_rotationSpeedOfLights;

	CameraZXY m_camera;

	float m_fov;

	static Game *s_instance;
private:
	void UpdateSpringJoints();
};


inline bool Game::IsQuitting() const { return m_quitReason != QUITREASON_NONE; }
inline bool Game::IsRunning() const { return !IsQuitting(); }