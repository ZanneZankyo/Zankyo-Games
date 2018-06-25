#include "NetSession.hpp"
#include "NetMessage.hpp"
#include "NetConnection.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"



NetSession::NetSession()
	: m_myConnection(nullptr)
	, m_hostConnection(nullptr)
	, m_connectionList(nullptr)
	, m_state(SESSION_DISCONNECTED)
{
	
}

NetSession::~NetSession()
{
	for (auto definition : m_registeredMessages)
		delete definition;
}

void NetSession::JoinConnection(uint8_t index, NetConnection* connection)
{
	connection->m_connectionIndex = index;
	ASSERT_OR_DIE(index >= m_connections.size() || m_connections[index] == nullptr, "NetSession::JoinConnection: same index");
	if (index >= m_connections.size())
		m_connections.resize(index + 1, nullptr);
	m_connections[index] = connection;
	
	if (!m_connectionList)
		m_connectionList = connection;
	else
	{
		NetConnection* currentConnection = m_connectionList;
		while (currentConnection->m_next)
			currentConnection = currentConnection->m_next;
		currentConnection->m_next = connection;
		connection->m_prev = currentConnection;
	}
}

NetConnection* NetSession::GetConnection(uint8_t index)
{
	if (index < m_connections.size())
		return m_connections[index];
	return nullptr;
}

void NetSession::SendMessageToOthers(NetMessage& message)
{
	for (auto connection : m_connections)
		if (connection && connection != m_myConnection)
			connection->Send(&message);
}

void NetSession::SendMessageToAll(NetMessage& message)
{
	for (auto connection : m_connections)
		if (connection)
			connection->Send(&message);
}

void NetSession::SendMessageToId(NetMessage& message, uint8_t connectionId)
{
	for (auto connection : m_connections)
		if (connection && connection->m_connectionIndex == connectionId)
			connection->Send(&message);
}

bool NetSession::IsRunning() const
{
	return m_myConnection != nullptr;
}

bool NetSession::IsReady() const
{
	return m_state == SESSION_CONNECTED;
}

uint8_t NetSession::GetFreeConnectionIndex()
{
	if (IsRunning() && IsHost())
	{
		for (size_t index = 0; index < m_connections.size(); index++)
		{
			if (m_connections[index] == nullptr)
				return (uint8_t)index;
		}
		//if (m_connections.size() < m_maxConnectionCount)
			return (uint8_t)m_connections.size();
	}
	return INVALID_CONNECTION_INDEX;
}

void NetSession::SetStatus(SessionState newState)
{
	m_state = newState;
}

bool NetSession::RegisterMessageEvent(uint8_t messageId, NetMessageEvent callback)
{
	m_registeredMessages.push_back(new NetMessageDefinition(messageId, callback));
	return true;
}

std::vector<const NetMessageDefinition*> NetSession::GetMessageEvent(uint8_t messageId) const
{
	std::vector<const NetMessageDefinition*> results;
	for (auto definition : m_registeredMessages)
	{
		if (definition->m_typeIndex == messageId)
			results.push_back(definition);
	}
	return results;
}

void NetSession::ProcessNetMessage(NetMessage* message)
{

	std::vector<const NetMessageDefinition*> definitions = GetMessageEvent(message->m_messageTypeIndex);
	for(auto definition : definitions)
		definition->Trigger(message);
	SAFE_DELETE(message);
}

void NetSession::DestroyConnection(NetConnection* connection)
{
	if (nullptr == connection) {
		return;
	}

	if (m_myConnection == connection) {
		m_myConnection = nullptr;
	}

	if (m_hostConnection == connection) {
		m_hostConnection = nullptr;
	}

	if (connection->m_connectionIndex != INVALID_CONNECTION_INDEX) {
		m_connections[connection->m_connectionIndex] = nullptr;
		connection->m_connectionIndex = INVALID_CONNECTION_INDEX;
	}

	RemoveFromList(connection);

	delete connection;
}

void NetSession::RemoveFromList(NetConnection* connection)
{
	if (connection == nullptr || m_connectionList == nullptr)
		return;
	if (m_connectionList == connection)// || connection->m_prev == nullptr (connection is head/host)
		m_connectionList = connection->m_next;
	else
		connection->m_prev->m_next = connection->m_next;
	if (connection == m_hostConnection)
		m_hostConnection = nullptr;
	for (size_t index = 0; index < m_connections.size(); index++)
	{
		if (m_connections[index] && m_connections[index] == connection)
		{
			m_connections[index] = nullptr;
			break;
		}
	}
}

uint8_t NetSession::GetIndexFromConnection(NetConnection * connection)
{
	for (size_t index = 0; index < m_connections.size(); index++)
	{
		if (m_connections[index] == connection)
			return (uint8_t)index;
	}
	return INVALID_CONNECTION_INDEX;
}