#pragma once
#include <vector>
#include "Time.hpp"
#include "Event.hpp"
#include "Timeline.hpp"

struct TimeEvent
{
	friend class EventTimeLine;
public:
	float m_time;
	Event<> m_event;
	void operator() ()
	{
		m_event.Trigger();
	}
	TimeEvent() {}
	TimeEvent(float time, Event<> event) : m_time(time), m_event(event) {}
	TimeEvent(const TimeEvent& copy) : m_time(copy.m_time), m_event(copy.m_event) {}
};



class EventTimeLine
{

public:
	float m_prevTime = 0.f;
	float m_currentTime = 0.f;

private:
	std::vector<TimeEvent> m_keys;
	
public:

	void Reset() 
	{ 
		m_prevTime = 0.f; 
		m_currentTime = 0.f; 
	}
	void Update(float deltaSeconds);
	void Update() 
	{ 
		Update(Time::deltaSeconds); 
	};
	void AddEvent(float time, Event<> event)
	{ 
		TimeEvent keyframe;
		keyframe.m_time = time;
		keyframe.m_event = event;
		m_keys.push_back(keyframe);
	}
	void SetEventInstance(void* instance)
	{
		for (auto& key : m_keys)
		{
			for (auto& reg : key.m_event.m_registrations)
			{
				reg.userArgument = instance;
			}
		}
	}
};