#pragma once

#include "Engine/Core/EngineBase.hpp"
#include <vector>
#include <string>
#include "Engine/Core/Win32Include.hpp"
#include "Engine/Network/Network.hpp"

class TCPSocket
{
public:
	SOCKET m_socket;
	net_address_t m_address;
	bool m_isListenSocket;
public:

	TCPSocket();
	~TCPSocket();

	//Client
	bool Join(const std::string& addrString);
	bool Join(net_address_t addr);

	//Host
	bool Host(uint16_t port);

	//Both
	uint Send(void const * payload, uint size);
	uint Send(const std::string& message);
	uint Receive(void *payload, uint maxSize);
	bool Listen(uint16_t port);
	void SetBlocking(bool isBlocking);
	bool CheckForDisconnect();
	bool IsListening();

	TCPSocket* Accept();

	bool IsValid() const;
	void Close();
	
};


/*

bool NetAddressFromSocketAddress(net_address_t *out, sockaddr *socketAddress);
void SocketAddressFromNetAddress(sockaddr* out, int *outSize, net_address_t const & addr);
std::string NetAddressToString(const net_address_t& addr);
net_address_t StringToNetAddress(const std::string& string);*/