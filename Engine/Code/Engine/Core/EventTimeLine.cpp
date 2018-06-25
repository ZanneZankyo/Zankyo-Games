#include "EventTimeLine.hpp"

void EventTimeLine::Update(float deltaSeconds)
{
	m_currentTime += deltaSeconds;

	//for (TimeEvent& event : m_keys)
	for (uint keyIndex = 0; keyIndex < m_keys.size(); keyIndex++)
	{
		TimeEvent& event = m_keys[keyIndex];
		if (m_prevTime <= event.m_time && event.m_time < m_currentTime)
			event();
	}
		

	m_prevTime = m_currentTime;
}

