#pragma once
#include "Network.hpp"
#include "NetConnection.hpp"
#include <deque>

class LookBackConnection : public NetConnection
{
public:
	std::deque<NetMessage*> m_messages;
public:
	LookBackConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex = INVALID_CONNECTION_INDEX);

	virtual void Send(NetMessage* message) override;
	virtual void Receive(NetMessage ** message) override;
	virtual std::vector<NetMessage *> Receive() override;
	virtual bool Connect() override;
	virtual bool IsValid() override;
};