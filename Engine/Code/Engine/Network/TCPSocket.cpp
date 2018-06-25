#include "TCPSocket.hpp"
#include "Engine\Core\StringUtils.hpp"
#include <WS2tcpip.h>
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Core\Console.hpp"


TCPSocket::TCPSocket()
	: m_socket(INVALID_SOCKET)
	, m_isListenSocket(false)
{}

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Join(net_address_t addr)
{
	if (IsValid())
		return false;
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return false;
	sockaddr_storage internalAddress;
	int addrSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&internalAddress, &addrSize, addr);
	int result = ::connect(sock, (sockaddr*)&internalAddress, sizeof(sockaddr_in)); // sizeof(sockaddr_in) only for IPV4
	if (result != 0)
	{
		::closesocket(sock);
		return false;
	}
	m_socket = sock;
	m_address = addr;
	return true;
}

bool TCPSocket::Join(const std::string& addrString)
{
	net_address_t addr = StringToNetAddress(addrString);
	return Join(addr);
}

uint TCPSocket::Send(void const * payload, uint size)
{
	if (!IsValid() || size == 0)
		return 0;
	int byteSent = ::send(m_socket, (char const*)payload, (int)size, 0);
	if (byteSent <= 0)
	{
		int error = WSAGetLastError();
		Console::Log("TCPSocket::Send failed. Error: " + std::to_string(error));
		Close();
		return 0;
	}
	ASSERT_OR_DIE(byteSent == (int)size, "TCPSocket::Send size and byteSent not match.\n");
	return byteSent;
}

uint TCPSocket::Send(const std::string& message)
{
	return Send(message.c_str(), message.length());
}

uint TCPSocket::Receive(void *payload, uint maxSize)
{
	if (!IsValid() || (maxSize == 0))
		return 0;
	ASSERT_OR_DIE(payload != nullptr, "TCPSocket::Receive payload is nullptr.\n");

	int byteRead = ::recv(m_socket, (char*)payload, (int)maxSize, 0);
	if (byteRead <= 0)
	{
		if (byteRead == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
				return 0;
			else
				Close();
		}
		else
			CheckForDisconnect();
		return 0;
	}
	else {
		return (uint)byteRead;
	}
}

bool TCPSocket::Listen(uint16_t port)
{
	if (IsValid())
		return false;
	std::vector<net_address_t> addresses = GetAddressesFromHostName(std::string(""), port, true);
	if (addresses.empty())
		return false;
	SOCKET listenSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
		return false;
	sockaddr_storage listenAddress;
	int addrSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&listenAddress, &addrSize, addresses[0]);
	int result = ::bind(listenSocket, (const sockaddr*)&listenAddress, (int)addrSize);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listenSocket);
		return false;
	}
	int maxQueued = 8;
	result = ::listen(listenSocket, (int)maxQueued);
	if (result == SOCKET_ERROR)
	{
		::closesocket(listenSocket);
		return false;
	}
	m_socket = listenSocket;
	NetAddressFromSocketAddress(&m_address, (sockaddr*)&listenAddress);
	m_isListenSocket = true;
	return true;
}

void TCPSocket::SetBlocking(bool isBlocking)
{
	if (!IsValid())
		return;
	u_long nonBlocking = isBlocking ? 0 : 1;
	::ioctlsocket(m_socket, FIONBIO, &nonBlocking);
}

bool TCPSocket::IsValid() const
{
	return (m_socket != INVALID_SOCKET);
}

void TCPSocket::Close()
{
	if (IsValid())
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

bool TCPSocket::CheckForDisconnect()
{
	if (!IsValid())
		return false;
	WSAPOLLFD fd;
	fd.fd = m_socket;
	fd.events = POLLRDNORM;

	if (SOCKET_ERROR == WSAPoll(&fd, 1, 0))
		return false;
	if ((fd.revents & POLLHUP) != 0)
	{
		Close();
		return true;
	}
	return false;
}

bool TCPSocket::IsListening()
{
	return m_isListenSocket;
}

TCPSocket* TCPSocket::Accept()
{
	if (!IsListening())
		return nullptr;
	sockaddr_storage connectAddr;
	int connectAddrLen = sizeof(connectAddr);

	//int newAddrLen = 0;
	SOCKET newSocket = ::accept(m_socket, (sockaddr*)& connectAddr, &connectAddrLen);

	if (newSocket == INVALID_SOCKET)
	{
		return nullptr;
	}

	net_address_t netAddr;
	if (!NetAddressFromSocketAddress(&netAddr, (sockaddr*)&connectAddr))
	{
		::closesocket(newSocket);
		return nullptr;
	}

	TCPSocket* newTcpSocket = new TCPSocket();
	newTcpSocket->m_socket = newSocket;
	newTcpSocket->m_address = netAddr;

	return newTcpSocket;
}
