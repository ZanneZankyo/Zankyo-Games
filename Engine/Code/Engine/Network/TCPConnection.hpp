#pragma once
#include "NetworkType.hpp"
#include "NetConnection.hpp"
#include <vector>

class TCPConnection : public NetConnection
{
public:
	TCPSocket* m_socket;
	BinaryStream m_buffer;
public:
	TCPConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex = INVALID_CONNECTION_INDEX);
	virtual void Send(NetMessage* message) override;
	virtual void Receive(NetMessage ** message) override;
	virtual std::vector<NetMessage *> Receive() override;
	virtual bool Connect() override;
	virtual bool IsValid() override;
};