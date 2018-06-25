#pragma once
#include "NetworkType.hpp"
#include <functional>

//typedef void(*NetMessageEvent)(NetMessage*, NetSession*);
typedef std::function<void(NetMessage*)> NetMessageEvent;

class NetMessageDefinition
{
public:
	uint8_t m_typeIndex;
	NetMessageEvent m_handler;
public:
	NetMessageDefinition() {}
	NetMessageDefinition(uint8_t typeIndex, NetMessageEvent handler) : m_typeIndex(typeIndex), m_handler(handler) {}
	NetMessageDefinition(const NetMessageDefinition& copy) : m_typeIndex(copy.m_typeIndex), m_handler(copy.m_handler) {}
	void Trigger(NetMessage* message) const { m_handler(message); }
};