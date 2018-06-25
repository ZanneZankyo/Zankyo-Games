#pragma once
#include "Network.hpp"
#include "NetMessage.hpp"
#include "NetSession.hpp"

class NetConnection
{
public:
	NetConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex);
	virtual ~NetConnection();
	virtual void Send(NetMessage* message) = 0;
	virtual void Receive(NetMessage ** message) = 0;
	virtual std::vector<NetMessage*> Receive() = 0;
	virtual bool Connect() = 0;
	virtual bool IsValid() = 0;
public:

	NetConnection* m_prev;
	NetConnection* m_next;

	NetSession* m_owner;
	net_address_t m_address;
	uint8_t m_connectionIndex;
};