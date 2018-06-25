#pragma once
#include "NetworkType.hpp"
#include <map>

typedef void (*NetObjectWriteCallBack)(NetMessage* message, void* object);
typedef void* (*NetObjectReadCallBack)(NetMessage* message, NetObject *object, float deltaSeconds);
typedef uint8_t net_object_definition_id_t;

class NetObjectDefinition
{
public:
	NetObjectDefinition() {};
	NetObjectDefinition
	(
		net_object_definition_id_t typeId, 
		NetObjectWriteCallBack writeCallback, 
		NetObjectWriteCallBack writeDeltaCallback,
		NetObjectReadCallBack readCallBack, 
		NetObjectReadCallBack readDeltaCallBack,
		size_t byteSize
	)
		: m_typeId(typeId)
		, m_writeCallBack(writeCallback)
		, m_writeDeltaCallBack(writeDeltaCallback)
		, m_readCallBack(readCallBack)
		, m_readDeltaCallBack(readDeltaCallBack)
		, m_byteSize(byteSize) 
	{}
public:
	uint8_t m_typeId;
	NetObjectWriteCallBack m_writeCallBack;
	NetObjectWriteCallBack m_writeDeltaCallBack;
	NetObjectReadCallBack m_readCallBack;
	NetObjectReadCallBack m_readDeltaCallBack;
	size_t m_byteSize;
public:
	static void RegisterDefinition(net_object_definition_id_t typeId, NetObjectDefinition* definition);
	static void RegisterDefinition
	(
		net_object_definition_id_t typeId, 
		NetObjectWriteCallBack writeCallback,
		NetObjectWriteCallBack writeDeltaCallback,
		NetObjectReadCallBack readCallback,
		NetObjectReadCallBack readDeltaCallback,
		size_t byteSize
	);
	static NetObjectDefinition* GetDefinition(net_object_definition_id_t typeId);
public:
	static std::map<net_object_definition_id_t, NetObjectDefinition*> s_definitions;
};