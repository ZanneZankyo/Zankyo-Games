#include "Network.hpp"

#include "Engine/Core/Win32Include.hpp"
#include <WinSock2.h>
#include "Engine\Core\StringUtils.hpp"
#include <WS2tcpip.h>
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\Console.hpp"

bool NetSystemStart()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	int error = ::WSAStartup(version, &data);
	return (error == 0);
}

void NetSystemShutdown()
{
	::WSACleanup();
}

std::vector<net_address_t> GetAddressesFromHostName(
	std::string& hostName,
	uint16_t port,
	bool bindable
)
{
	std::string service = std::to_string(port);

	addrinfo hints;
	MemSetZero(&hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // udp = SOCK_DGRAM;
	hints.ai_flags |= (bindable ? AI_PASSIVE : 0);
	//hints.ai_flags |= AI_NUMERICHOST; // if only receive ip address

	addrinfo *results = nullptr;

	int status = ::getaddrinfo(hostName.c_str(), service.c_str(), &hints, &results);

	std::vector<net_address_t> addresses;
	if (status != 0)
	{
		return addresses;
	}

	addrinfo *addr = results;
	while (addr != nullptr)
	{
		net_address_t netAddr;
		if (NetAddressFromSocketAddress(&netAddr, addr->ai_addr))
			addresses.push_back(netAddr);
		addr = addr->ai_next;
	}

	::freeaddrinfo(results);
	return addresses;

}

bool NetAddressFromSocketAddress(net_address_t *out, sockaddr *socketAddress)
{
	if (socketAddress == nullptr)
		return false;

	if (socketAddress->sa_family != AF_INET) // assume that it only support IPV4
		return false;

	sockaddr_in * ipv4Address = (sockaddr_in*)socketAddress;
	out->port = ntohs(ipv4Address->sin_port);
	out->address = ntohl(ipv4Address->sin_addr.S_un.S_addr);

	return true;
}

void SocketAddressFromNetAddress(sockaddr* out, int *outSize, net_address_t const & addr)
{
	sockaddr_in *outAddr = (sockaddr_in*)out;
	MemSetZero(outAddr);
	outAddr->sin_family = AF_INET;
	outAddr->sin_port = htons(addr.port);
	outAddr->sin_addr.S_un.S_addr = htonl(addr.address);

	*outSize = sizeof(sockaddr_in);
}

std::string NetAddressToString(const net_address_t& addr)
{
	unsigned char *ips = (unsigned char*)&addr.address;
	unsigned char a = ips[3];
	unsigned char b = ips[2];
	unsigned char c = ips[1];
	unsigned char d = ips[0];
	return std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d) + ":" + std::to_string(addr.port);
}

net_address_t StringToNetAddress(const std::string& string)
{
	std::pair<std::string, std::string> ipStrAndPort = StringUtils::SplitAtLastToken(string, ':');
	std::vector<std::string> ipString = StringUtils::Split(ipStrAndPort.first, '.');
	unsigned char ips[4] = {
		(unsigned char)stoi(ipString[3]),
		(unsigned char)stoi(ipString[2]),
		(unsigned char)stoi(ipString[1]),
		(unsigned char)stoi(ipString[0])
	};
	uint* ipAddr = (uint*)ips;
	uint16_t port = (uint16_t)stoi(ipStrAndPort.second);
	net_address_t addr;
	addr.address = *ipAddr;
	addr.port = port;
	return addr;
}

net_address_t GetMyAddress(uint16_t port)
{
	std::string service = std::to_string(port);

	addrinfo hints;
	MemSetZero(&hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; // udp = SOCK_DGRAM;
	hints.ai_flags |= AI_PASSIVE;
	//hints.ai_flags |= AI_NUMERICHOST; // if only receive ip address

	addrinfo *results = nullptr;

	int status = ::getaddrinfo("", service.c_str(), &hints, &results);

	if (status != 0)
	{
		return net_address_t();
	}

	addrinfo *addr = results;
	net_address_t netAddr;
	NetAddressFromSocketAddress(&netAddr, addr->ai_addr);
	if(port)
		netAddr.port = port;
	::freeaddrinfo(results);
	return netAddr;
}
