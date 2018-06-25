#pragma once
#include <vector>

template <typename DataType>
struct KeyFrame
{
	float m_time;
	DataType m_value;

	KeyFrame() 
		: m_time(0.0f), m_value() {}

	KeyFrame(float time, const DataType& data) 
		: m_time(time), m_value(data) {}

	float& Time() 
	{ return m_time; }

	const float& Time() const 
	{ return m_time; }

	DataType& Value() 
	{ return m_value; }

	const DataType& Value() const 
	{ return m_value; }

	bool operator < (const KeyFrame& comp) 
	{ return m_time < comp.m_time; }
};

template <typename DataType>
class Timeline
{
private:
	std::vector<KeyFrame<DataType>> m_keys;
public:
	Timeline() : m_keys() {}
	Timeline(const Timeline& copy) : m_keys(copy.m_keys) {}
	void AddKey(float time, const DataType& value);
	DataType& AtTime(float time);
	DataType AtTime(float time) const;
};

template <typename DataType>
void Timeline<DataType>::AddKey(float time, const DataType& value)
{
	KeyFrame<DataType> newKey(time, value);
	if (m_keys.empty())
	{
		m_keys.push_back(newKey);
		return;
	}
	if (m_keys[0].Time() > time)
	{
		m_keys.insert(m_keys.begin(), newKey);
		return;
	}
	for (size_t index = 0; index < m_keys.size(); index++)
	{
		size_t next = index + 1;
		if (next >= m_keys.size())
		{
			m_keys.push_back(newKey);
			return;
		}
		if (m_keys[index].Time() <= time && time <= m_keys[next].Time())
		{
			m_keys.insert(m_keys.begin() + next, newKey);
			return;
		}
	}
}

template <typename DataType>
DataType Timeline<DataType>::AtTime(float time) const
{
	int frontIndex = -1;
	int endIndex = -1;
	//bool evaluated = false;

	if (m_keys.empty())
		return DataType();

	if (m_keys[0].Time() >= time)
		return m_keys[0].Value();
	if (m_keys[m_keys.size() - 1].Time() <= time)
		return m_keys[m_keys.size() - 1].Value();

	int leftIndex = 0;
	int rightIndex = m_keys.size() - 1;
	int currentIndex = (leftIndex + rightIndex) / 2;
	while (leftIndex != rightIndex)
	{
		float leftTime = m_keys[currentIndex].Time();
		float rightTime = m_keys[currentIndex + 1].Time();
		if (leftTime <= time && time <= rightTime)
		{
			float fraction = RangeMap(
				time, leftTime,
				rightTime,
				0.0, 1.0);
			return Interpolate(
				m_keys[currentIndex].Value(),
				m_keys[currentIndex + 1].Value(),
				fraction);
		}
		else if (leftTime > time)
			rightIndex = currentIndex;
		else
			leftIndex = currentIndex;
		currentIndex = (leftIndex + rightIndex) / 2;
	}


	if (frontIndex != -1)
		return m_keys[frontIndex].Value();
	else if (endIndex != -1)
		return m_keys[endIndex].Value();
	else
		return DataType();
}

template <typename DataType>
DataType& Timeline<DataType>::AtTime(float time)
{
	int frontIndex = -1;
	int endIndex = -1;
	bool evaluated = false;

	if (m_keys.empty())
		return DataType();

	if (m_keys[0].Time() >= time)
		return m_keys[0].Value();
	if (m_keys[m_keys.size() - 1].Time() <= time)
		return m_keys[m_keys.size() - 1].Value();

	int leftIndex = 0;
	int rightIndex = m_keys.size() - 1;
	int currentIndex = (leftIndex + rightIndex) / 2;
	while (leftIndex != rightIndex)
	{
		float leftTime = m_keys[currentIndex].Time();
		float rightTime = m_keys[currentIndex + 1].Time();
		if (leftTime <= time && time <= rightTime)
		{
			float fraction = RangeMap(
				time, leftTime,
				rightTime,
				0.0, 1.0);
			return Interpolate(
				m_keys[currentIndex].Value(),
				m_keys[currentIndex + 1].Value(),
				fraction);
		}
		else if (leftTime > time)
			rightIndex = currentIndex;
		else
			leftIndex = currentIndex;
		currentIndex = (leftIndex + rightIndex) / 2;
	}


	if (frontIndex != -1)
		return m_keys[frontIndex].Value();
	else if (endIndex != -1)
		return m_keys[endIndex].Value();
	else
		return DataType();
}