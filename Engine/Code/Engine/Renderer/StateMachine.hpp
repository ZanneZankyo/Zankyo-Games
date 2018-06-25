#pragma once
#include <map>
#include <string>
#include "Motion.hpp"
#include "Engine\Core\EventTimeLine.hpp"
#include "Skeleton.hpp"

struct StateMachineState
{
	std::string name = "";
	Motion* motion = nullptr;
	float transitionTime = 0;
	EventTimeLine timeline;
};

class StateMachine
{
public:
	std::map<std::string, StateMachineState*> m_motions;
	StateMachineState* m_entryState = nullptr;
public:
	~StateMachine();
	void SetDefaultMotion(const std::string& name);
	void AddState(const std::string& name, Motion* motion, float transitionTime = 0);
	void AddMotionEvent(const std::string& stateName, float time, Event<> event);
	float GetDuration(const std::string& stateName);
};

class StateMachineInstance
{
public:
	SkeletonInstance* m_skeletonInstance = nullptr;
	StateMachine* m_machine = nullptr;
	StateMachineState* m_currentState = nullptr;
	StateMachineState* m_prevState = nullptr;
	float m_prevTime = 0;
	float m_currentTime = 0;
	float m_blendTime = 0;
	void* m_owner = nullptr;
	Vector3 m_rootOffset;
public:
	StateMachineInstance(StateMachine* machine, Skeleton* skeleton)
		: m_skeletonInstance(new SkeletonInstance(skeleton))
		, m_machine(machine)
		, m_currentState(new StateMachineState(*m_machine->m_entryState))
		, m_prevState(nullptr)
		, m_prevTime(0.f)
		, m_currentTime(0.f)
		, m_blendTime(0.f)
		, m_owner(nullptr)
		, m_rootOffset()
	{}
	~StateMachineInstance()
	{
		SAFE_DELETE(m_skeletonInstance);
		SAFE_DELETE(m_currentState);
		SAFE_DELETE(m_prevState);
	}
	void Update(Vector3* syncPosition = nullptr);
	void SetState(const std::string& stateName);
	void SetOwner(void* actor);
	void EvaluateSkinning();
};