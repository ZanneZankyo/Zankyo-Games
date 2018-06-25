#pragma once
#include <vector>
#include "Network.hpp"
#include "NetMessageDefinition.hpp"
#include <map>

enum SessionState
{
	SESSION_DISCONNECTED,
	SESSION_CONNECTING,
	//SESSION_JOINING,
	SESSION_CONNECTED,
	//SESSION_JOINED
};

class NetSession
{
public:
	NetConnection* m_connectionList;
	std::vector<NetConnection*> m_connections;
	NetConnection* m_myConnection;
	NetConnection* m_hostConnection;

	std::vector<NetMessageDefinition*> m_registeredMessages;

	SessionState m_state;

	static constexpr uint8_t INVALID_CONNECTION_INDEX = 0xff;

public:
	NetSession();
	virtual ~NetSession();
public:
	virtual bool Host(uint16_t port) = 0;
	virtual bool Join(const net_address_t& address) = 0;
	virtual void Leave() = 0;
	virtual void Update() = 0;
	void JoinConnection(uint8_t index, NetConnection* connection);
	NetConnection* GetConnection(uint8_t index);
	void SendMessageToOthers(NetMessage& message);
	void SendMessageToAll(NetMessage& message);
	void SendMessageToId(NetMessage& message, uint8_t connectionId);
	bool IsRunning() const;
	bool IsReady() const;
	uint8_t GetFreeConnectionIndex();
	uint8_t GetIndexFromConnection(NetConnection* connection);
	void SetStatus(SessionState newState);
	bool RegisterMessageEvent(uint8_t messageId, NetMessageEvent callback);
	std::vector<const NetMessageDefinition*> GetMessageEvent(uint8_t messageId) const;
	void ProcessNetMessage(NetMessage* message);
	void DestroyConnection(NetConnection* connection);
	void RemoveFromList(NetConnection* connection);
	bool HasMessageEvent(uint8_t messageId);
	void UnRegisterMessageEvent(uint8_t messageId);
	inline bool IsHost() const
	{
		return m_myConnection && m_myConnection == m_hostConnection;
	}
	inline bool IsClient() const
	{
		return m_myConnection && m_myConnection != m_hostConnection;
	}

};