#pragma once
#include "NetSession.hpp"
#include "Network.hpp"

class TCPSession : public NetSession
{
public:
	TCPSocket* m_listenSocket;
public:
	TCPSession();
	virtual ~TCPSession();
public:
	virtual bool Host(uint16_t port) override;
	virtual bool Join(const net_address_t& address) override;
	virtual void Leave() override;
	virtual void Update() override;
	bool StartListening();
	void StopListening();
	bool IsListening() const;
	void SendJoinResponse(NetConnection* connection);
	void OnJoinResponse(NetMessage* message);
public:
	
};