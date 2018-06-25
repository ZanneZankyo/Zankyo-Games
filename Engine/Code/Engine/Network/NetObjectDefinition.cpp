#include "NetObjectDefinition.hpp"

std::map<net_object_definition_id_t, NetObjectDefinition*> NetObjectDefinition::s_definitions;

void NetObjectDefinition::RegisterDefinition(net_object_definition_id_t typeId, NetObjectDefinition* definition)
{
	auto found = s_definitions.find(typeId);
	if (found != s_definitions.end())
		SAFE_DELETE(found->second);
	s_definitions[typeId] = definition;
}

void NetObjectDefinition::RegisterDefinition
(
	net_object_definition_id_t typeId,
	NetObjectWriteCallBack writeCallback,
	NetObjectWriteCallBack writeDeltaCallback,
	NetObjectReadCallBack readCallback,
	NetObjectReadCallBack readDeltaCallback,
	size_t byteSize
)
{
	RegisterDefinition
	(
		typeId, 
		new NetObjectDefinition
		(
			typeId, 
			writeCallback, 
			writeDeltaCallback, 
			readCallback, 
			readDeltaCallback, 
			byteSize
		)
	);
}

NetObjectDefinition* NetObjectDefinition::GetDefinition(net_object_definition_id_t typeId)
{
	auto found = s_definitions.find(typeId);
	if (found != s_definitions.end())
		return found->second;
	return nullptr;
}
