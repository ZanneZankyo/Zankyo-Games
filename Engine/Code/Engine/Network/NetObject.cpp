#include "NetObject.hpp"
#include "Engine\Core\Console.hpp"
#include "Engine\Core\EngineBase.hpp"
#include "NetMessage.hpp"
#include "NetObjectDefinition.hpp"

net_object_id_t NetObject::INVALID_NET_OBJECT_ID = (net_object_id_t)-1;
std::vector<NetObject*> NetObject::s_netObjects;
std::vector<NetObject*> NetObject::s_oldObjects;

NetObject::NetObject()
	: m_netId(INVALID_NET_OBJECT_ID)
	, m_definition(nullptr)
	, m_localObject(nullptr)
{

}

NetObject::NetObject(net_object_id_t netId, NetObjectDefinition* definition, void* localObject)
	: m_netId(netId)
	, m_definition(definition)
	, m_localObject(localObject)
{

}

NetObject::NetObject(const NetObject& copy)
	: m_netId(copy.m_netId)
	, m_definition(copy.m_definition)
	, m_localObject(copy.m_localObject)
{

}

void NetObject::Write(NetMessage* message)
{
	message->WriteBuffer(m_netId);
	message->WriteBuffer(m_definition->m_typeId);
	m_definition->m_writeCallBack(message, m_localObject);
}

void NetObject::WriteDelta(NetMessage* message)
{
	m_definition->m_writeDeltaCallBack(message, m_localObject);
}

void NetObject::Read(NetMessage* message, float deltaSeconds)
{
	m_netId = message->ReadBuffer<net_object_id_t>();
	net_object_definition_id_t defId = message->ReadBuffer<net_object_definition_id_t>();
	m_definition = NetObjectDefinition::GetDefinition(defId);
	if (!m_definition)
		return;
	m_definition->m_readCallBack(message, this, 0.f);
}

void NetObject::ReadDelta(NetMessage* message, float deltaSeconds /*= 0.f*/)
{
	if (!m_definition)
		return;
	m_definition->m_readDeltaCallBack(message, this, 0.f);
}

void NetObject::OverWrite(NetObject* object)
{
	memcpy(m_localObject, object->m_localObject, m_definition->m_byteSize);
}

NetObject* NetObject::GetObjectByPointer(void* pointer)
{
	for (auto object : s_netObjects)
		if (object && object->m_localObject == pointer)
			return object;
	return nullptr;
}

NetObject* NetObject::GetOldObjectByPointer(void* pointer)
{
	for (auto object : s_netObjects)
		if (object && object->m_localObject == pointer)
			return GetOldObjectById(object->m_netId);
	return nullptr;
}

void NetObject::DestroyObject(net_object_id_t netId)
{
	if (netId >= s_netObjects.size())
		return;
	NetObject* object = s_netObjects[netId];
	s_netObjects[netId] = nullptr;
	SAFE_DELETE(object);

	NetObject* duplicate = s_oldObjects[netId];
	if (duplicate)
		SAFE_DELETE(duplicate->m_localObject);
	SAFE_DELETE(duplicate);
}

NetObject * NetObject::Duplicate(NetObject * object)
{
	if (!object || !object->m_definition || !object->m_localObject)
		return nullptr;
	NetObject* copy = new NetObject();
	copy->m_netId = object->m_netId;
	copy->m_definition = object->m_definition;
	copy->m_localObject = new byte_t[object->m_definition->m_byteSize];
	memcpy(copy->m_localObject, object->m_localObject, object->m_definition->m_byteSize);
	return copy;
}

net_object_id_t NetObject::GetFreeNetObjectId()
{
	if (s_netObjects.empty())
		s_netObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	if (s_oldObjects.empty())
		s_oldObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	for (net_object_id_t index = 0; index < s_netObjects.size(); index++)
	{
		if (s_netObjects[index] == nullptr)
			return index;
	}
	return (net_object_id_t)-1;
}

NetObject* NetObject::NewNetObject(NetObjectDefinition *definition, void* localObject)
{
	if (s_netObjects.empty())
		s_netObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	net_object_id_t newIndex = GetFreeNetObjectId();
	if (newIndex == INVALID_NET_OBJECT_ID)
		return nullptr;
	NetObject* newObject = new NetObject(newIndex, definition, localObject);
	s_netObjects[newIndex] = newObject;
	s_oldObjects[newIndex] = Duplicate(newObject);
	return newObject;
}

void NetObject::NewNetObject(NetObject* object)
{
	if (s_netObjects.empty())
		s_netObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	if (s_oldObjects.empty())
		s_oldObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	if (!object)
		return;
	if (object->m_netId >= INVALID_NET_OBJECT_ID)
		return;
	if (s_netObjects[object->m_netId])
		Console::Log("NetObject::NewNetObject(NetObject* object): existed object in slot", Console::WARNING_COLOR);
	SAFE_DELETE(s_netObjects[object->m_netId]);

	s_netObjects[object->m_netId] = object;
	s_oldObjects[object->m_netId] = Duplicate(object);
}

void NetObject::UpdateOld()
{
	for (net_object_id_t netId = 0; netId < INVALID_NET_OBJECT_ID && netId < s_netObjects.size(); netId++)
	{
		NetObject* currentObject = s_netObjects[netId];
		NetObject* oldObject = s_oldObjects[netId];
		if (currentObject && currentObject->m_localObject && oldObject && oldObject->m_localObject)
			memcpy(oldObject->m_localObject, currentObject->m_localObject, currentObject->m_definition->m_byteSize);
	}
}

NetObject* NetObject::GetObjectById(net_object_id_t objectId)
{
	if (objectId >= s_netObjects.size())
		return nullptr;
	return s_netObjects[objectId];
}

NetObject* NetObject::GetOldObjectById(net_object_id_t objectId)
{
	if (objectId >= s_oldObjects.size())
		return nullptr;
	return s_oldObjects[objectId];
}

net_object_id_t NetObject::GetObjectSize()
{
	net_object_id_t size = 0;
	for (auto object : s_netObjects)
		if (object)
			size++;
	return size;
}

void NetObject::WriteAll(NetMessage* message)
{
	message->WriteBuffer(GetObjectSize());
	for (auto object : s_netObjects)
		if(object)
			object->Write(message);
}

void NetObject::WriteAllDelta(NetMessage* message)
{
	message->WriteBuffer(GetObjectSize());
	for (auto object : s_netObjects)
		if (object)
			object->WriteDelta(message);
}

void NetObject::ReadAll(NetMessage* message, float deltaSeconds)
{
	if (s_netObjects.empty())
		s_netObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	net_object_id_t size = message->ReadBuffer<net_object_id_t>();
	for (net_object_id_t count = 0; count < size; count++)
	{
		NetObject* object = new NetObject();
		object->Read(message);
		if(!s_netObjects[object->m_netId])
			NewNetObject(object);
		else
		{
			s_netObjects[object->m_netId]->OverWrite(object);
			SAFE_DELETE(object);
		}
	}
}

void NetObject::ReadAllDelta(NetMessage* message)
{
	if (s_netObjects.empty())
		s_netObjects.resize(INVALID_NET_OBJECT_ID, nullptr);
	net_object_id_t size = message->ReadBuffer<net_object_id_t>();
	for (net_object_id_t index = 0; index < size; index++)
	{
		NetObject* object = s_netObjects[index];
		if(object)
			object->ReadDelta(message);
	}
}
