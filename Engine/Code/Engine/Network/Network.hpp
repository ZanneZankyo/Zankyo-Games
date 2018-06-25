#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "NetworkType.hpp"
#include <vector>

bool NetSystemStart();

void NetSystemShutdown();

std::vector<net_address_t> GetAddressesFromHostName(
	std::string& hostName,
	uint16_t port,
	bool bindable
);
bool NetAddressFromSocketAddress(net_address_t *out, sockaddr *socketAddress);
void SocketAddressFromNetAddress(sockaddr* out, int *outSize, net_address_t const & addr);
std::string NetAddressToString(const net_address_t& addr);
net_address_t StringToNetAddress(const std::string& string);
net_address_t GetMyAddress(uint16_t port = 0);