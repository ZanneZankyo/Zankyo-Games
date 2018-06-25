#pragma once
#include "NetworkType.hpp"
#include <vector>

typedef uint16_t net_object_id_t;

class NetObject
{
public:
	NetObject();
	NetObject(net_object_id_t netId, NetObjectDefinition* definition, void* localObject);
	NetObject(const NetObject& copy);
	void Write(NetMessage* message);
	void WriteDelta(NetMessage* message);
	void Read(NetMessage* message, float deltaSeconds = 0.f);
	void ReadDelta(NetMessage* message, float deltaSeconds = 0.f);
	void OverWrite(NetObject* object);
public:
	net_object_id_t m_netId;
	NetObjectDefinition *m_definition;

	void *m_localObject;

public:
	static NetObject* Duplicate(NetObject* object);
	static net_object_id_t GetFreeNetObjectId();
	static void Backup();
	static NetObject* NewNetObject(NetObjectDefinition *definition, void* localObject);
	static NetObject* GetObjectById(net_object_id_t objectId);
	static NetObject* GetOldObjectById(net_object_id_t objectId);
	static NetObject* GetObjectByPointer(void* pointer);
	static NetObject* GetOldObjectByPointer(void* pointer);
	static void DestroyObject(net_object_id_t netId);
	static net_object_id_t GetObjectSize();
	static void WriteAll(NetMessage* message);
	static void WriteAllDelta(NetMessage* message);
	static void ReadAll(NetMessage* message, float deltaSeconds = 0.f);
	static void ReadAllDelta(NetMessage* message);
	static void NewNetObject(NetObject* object);
	static void UpdateOld();
public:
	static net_object_id_t INVALID_NET_OBJECT_ID;
	static std::vector<NetObject*> s_netObjects;
	static std::vector<NetObject*> s_oldObjects;
};