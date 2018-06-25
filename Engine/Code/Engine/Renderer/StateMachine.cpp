#include "StateMachine.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\Time.hpp"
#include "Engine\Core\Event.hpp"

StateMachine::~StateMachine()
{
	for (auto pair : m_motions)
		SAFE_DELETE(pair.second);
}

void StateMachine::SetDefaultMotion(const std::string& name)
{
	auto found = m_motions.find(name);
	if (found != m_motions.end())
		m_entryState = found->second;
}

void StateMachine::AddState(const std::string& name, Motion* motion, float transitionTime /*= 0*/)
{
	StateMachineState* newState = new StateMachineState();
	newState->name = name;
	newState->motion = motion;
	newState->transitionTime = transitionTime;
	if (m_motions.find(name) != m_motions.end())
	{
		ASSERT_RECOVERABLE(false, "StateMachine::AddState() same state name.");
		SAFE_DELETE(m_motions[name]);
	}
	m_motions[name] = newState;
}

void StateMachine::AddMotionEvent(const std::string& stateName, float time, Event<> event)
{
	auto found = m_motions.find(stateName);
	StateMachineState* state = found->second;
	state->timeline.AddEvent(time, event);
}

float StateMachine::GetDuration(const std::string& stateName)
{
	return m_motions[stateName]->motion->m_duration;
}

void StateMachineInstance::Update(Vector3* syncPosition/* = nullptr*/)
{
	m_currentTime += Time::deltaSeconds;
	m_prevTime += Time::deltaSeconds;
	m_blendTime -= Time::deltaSeconds;
	if (m_blendTime < 0)
		m_blendTime = 0;
	Pose& currentPose = m_skeletonInstance->m_currentPose;
	m_currentState->motion->EvaluateTimedAdditive(currentPose, m_currentTime);
	m_currentState->timeline.Update();
	if (m_prevState)
	{
		Pose prevPose;
		m_prevState->motion->EvaluateTimedAdditive(prevPose, m_prevTime);
		float fraction = m_prevState->transitionTime != 0.f ? m_blendTime / m_prevState->transitionTime : 0.f;
		for (int jointIndex = 0; jointIndex < (int)currentPose.m_transforms.size(); jointIndex++)
		{
			currentPose.GetTransform(jointIndex) = Interpolate(currentPose.GetTransform(jointIndex), prevPose.GetTransform(jointIndex),fraction);
		}
		//m_prevState->timeline.Update();
	}

	/*if (syncPosition)
	{
		Vector3 offset = currentPose.GetTransform(0).GetPosition();
		offset -= m_rootOffset;
		*syncPosition += offset;
	}*/
	//m_rootOffset = currentPose.GetTransform(0).GetPosition();
	//currentPose.GetTransform(0).SetTranslate(Vector3::ZERO);

	
}

void StateMachineInstance::SetState(const std::string& stateName)
{
	StateMachineState* newState = new StateMachineState(*m_machine->m_motions[stateName]);
	SAFE_DELETE(m_prevState);
	m_prevState = m_currentState;
	m_prevTime = m_currentTime;
	m_blendTime = newState->transitionTime;
	m_currentState = newState;
	m_currentTime = 0;
	if (m_currentState)
		m_currentState->timeline.SetEventInstance(m_owner);
	//m_rootOffset = Vector3::ZERO;
}

void StateMachineInstance::SetOwner(void* actor)
{
	m_owner = actor;
}

void StateMachineInstance::EvaluateSkinning()
{
	m_skeletonInstance->EvaluateSkinning();
}
