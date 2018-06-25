#pragma once
#include "Entity.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include <vector>
#include "Engine\Renderer\StateMachine.hpp"

class UnitychanDefinition
{
public:
	StateMachine m_stateMachine;
	Skeleton* m_skeleton = nullptr;
	std::string m_skeletonFilePath = "Data/bin/unitychan.skel";
	std::vector<Mesh*> *m_modelMeshSet = nullptr;
	std::string m_meshFilePath = "Data/bin/unitychan.mesh";
public:
	static UnitychanDefinition* s_instance;
public:
	~UnitychanDefinition();
public:
	static void Start();
	void SetUnityChanStateMachine();
	void SetUnityChanMaterial();
};

class Unitychan : public Entity
{
public:
	UnitychanDefinition* m_definition = UnitychanDefinition::s_instance;
	StateMachineInstance *m_state = nullptr;
	StructuredBuffer* m_poseBuffer = nullptr;
	bool m_isRunning = false;
	bool m_isInAction = false;
	bool m_isBlockingCommands = false;
	float m_runSpeed = 500.f;
public:
	Unitychan();
	~Unitychan();

	virtual void Update() override;
	virtual void Render() const override;

	Vector3 GetSocketPosition(const std::string& socketName);
	Matrix4 GetSocketTransform(const std::string& socketName);



	void CheckInputs();
	void CheckMovementInputs();
	void EndCommandBlocker();
	void EndAnimation();
	void Recover();

	void ActionKick();
	void ActionRecover();

	void ActionKickOnStartHitbox();
	void ActionKickOnEndHitbox();
private:
	void CheckAI();
};