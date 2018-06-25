#include "TCPSession.hpp"
#include "TCPSocket.hpp"
#include "TCPConnection.hpp"
#include "LookBackConnection.hpp"
#include "Engine\Core\Console.hpp"
#include <functional>
#include "NetMessageDefinition.hpp"

TCPSession::TCPSession()
	: NetSession()
	, m_listenSocket(nullptr)
{
	//RegisterMessageEvent(JOIN_RESPONSE, OnJoinResponse);
	//NetMessageEvent onCommandEvent = std::bind(&TCPSession::OnCommand, s_instance, std::placeholders::_1);
	NetMessageEvent onJoin = std::bind(&TCPSession::OnJoinResponse, this, std::placeholders::_1);
	RegisterMessageEvent(JOIN_RESPONSE, onJoin);
}

TCPSession::~TCPSession()
{
}

bool TCPSession::Host(uint16_t port)
{
	m_listenSocket = new TCPSocket();
	m_listenSocket->Listen(port);
	m_listenSocket->SetBlocking(false);
	LookBackConnection* connection = new LookBackConnection(this, m_listenSocket->m_address, 0);
	if (m_connections.size() <= 0)
		m_connections.resize(1, nullptr);
	m_connections[0] = connection;
	m_hostConnection = connection;
	m_myConnection = connection;
	return true;
}

bool TCPSession::Join(const net_address_t& address)
{
	TCPConnection* host = new TCPConnection(this,address);
	JoinConnection(0, host);
	m_hostConnection = host;
	if (!m_hostConnection->Connect())
	{
		Leave();
		return false;
	}
	m_myConnection = new LookBackConnection(this,GetMyAddress());
	m_myConnection->m_address = GetMyAddress();
	SetStatus(SESSION_CONNECTING);
	return true;
}

void TCPSession::Leave()
{
	DestroyConnection(m_myConnection);
	DestroyConnection(m_hostConnection);
	for (size_t index = 0; index < m_connections.size(); index++)
	{
		DestroyConnection(m_connections[index]);
	}
	StopListening();
	SetStatus(SESSION_DISCONNECTED);
}

void TCPSession::Update()
{
	if (IsHost())
	{
		TCPSocket *newSocket = m_listenSocket->Accept();
		if (newSocket)
		{
			newSocket->SetBlocking(false);
			Console::Log("Got new connection: " + NetAddressToString(newSocket->m_address));
			TCPConnection* newConnection = new TCPConnection(this, newSocket->m_address);
			newConnection->m_socket = newSocket;
			uint8_t newIndex = GetFreeConnectionIndex();
			Console::Log("Connection ID: " + std::to_string(newIndex));
			JoinConnection(newIndex, newConnection);
			SendJoinResponse(newConnection);
			//newConnection->Connect();
		}
	}

	for (size_t index = 0; index < m_connections.size(); index++)
	{
		NetConnection* connection = m_connections[index];
		if(connection == nullptr)
			continue;
		if (!connection->IsValid())
		{
			RemoveFromList(connection);
			delete connection;
			continue;
		}
		/*NetMessage* msg = nullptr;
		connection->Receive(&msg);
		if (msg)
		{
			ProcessNetMessage(msg);
		}	*/
		std::vector<NetMessage*> messages = connection->Receive();
		for(auto message : messages)
			ProcessNetMessage(message);
	}
}

bool TCPSession::StartListening()
{
	if (!IsHost())
		return false;
	m_listenSocket = new TCPSocket();
	bool result = m_listenSocket->Listen(m_hostConnection->m_address.port);
	if (result)
		m_listenSocket->SetBlocking(false);
	else
		delete m_listenSocket;
	return true;
}

void TCPSession::StopListening()
{
	if (IsListening()) {
		delete m_listenSocket;
		m_listenSocket = nullptr;
	}
}

bool TCPSession::IsListening() const
{
	return m_listenSocket && m_listenSocket->IsValid();
}

void TCPSession::SendJoinResponse(NetConnection* connection)
{
	NetMessage msg(JOIN_RESPONSE);
	msg.WriteBuffer((uint8_t)connection->m_connectionIndex);

	connection->Send(&msg);
}

void TCPSession::OnJoinResponse(NetMessage* message)
{
	uint8_t index = message->ReadBuffer<uint8_t>();
	JoinConnection(index, m_myConnection);
	SetStatus(SESSION_CONNECTED);
	Console::Log("Connection ID: " + std::to_string(index));
}

