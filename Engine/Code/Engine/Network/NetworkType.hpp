#pragma once
#include "Engine/Core/EngineBase.hpp"

#ifdef _WIN64
typedef uint64_t SOCKET;
#else
typedef uint SOCKET;
#endif

struct sockaddr;

constexpr uint8_t INVALID_CONNECTION_INDEX = 0xff;

struct net_address_t
{
	uint address;
	uint16_t port;
};

enum NetMessageCoreType : byte_t
{
	JOIN_RESPONSE = 0,
	PING,
	PONG,
	NUM_OF_NET_MESSAGE_CORE_TYPE
};

class NetConnection;
class NetMessage;
class NetMessageDefinition;
class Session;

class LookBackConnection;
class TCPSession;
class TCPSocket;
class TCPConnection;

class NetObject;
class NetObjectDefinition;

class RemoteCommandService;
