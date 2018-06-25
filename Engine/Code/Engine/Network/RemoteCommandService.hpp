#pragma once
#include "NetworkType.hpp"
#include "Network.hpp"
#include "TCPSession.hpp"

typedef void(*RemoteCommandFunc)(const std::string&);
constexpr uint16_t RCS_PORT = 8000;

enum NetMessageRemoteCommandServiceType : byte_t
{
	SEND_COMMAND = 32,
	SEND_MESSAGE,
	NUM_OF_NET_MESSAGE_RCS_TYPE
};

class RemoteCommandService
{
public:
	static void Setup();
	void OnCommand(NetMessage* message);
	void OnMessage(NetMessage* message);
	static void SendCommandToOthers(const std::string& commandAndArguments);
	static void SendCommandToAll(const std::string& commandAndArguments);
	static void SendBackMessage(const std::string& string);
	static void Update();
	static void Join(const net_address_t& address);
	static void Join(const std::string& address);
	static void SendRemoteCommand(const std::string& commandAndArguments);
	static void SendRemoteCommandToAll(const std::string& commandAndArguments);
	static void SendRemoteCommandToId(const std::string& idAndCommandAndArguments);

	static void SetAllowRemoteEcho(const std::string& boolString);

public:
	TCPSession m_session;
	TCPConnection* m_currentSender;
	bool m_allowRemoteEcho = true;
public:
	static RemoteCommandService* GetInstance();
	static RemoteCommandService* s_instance;
};

