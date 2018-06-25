#include "LookBackConnection.hpp"
#include "TCPSocket.hpp"

bool LookBackConnection::IsValid()
{
	return true;
}

LookBackConnection::LookBackConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex)
	: NetConnection(owner,address,connectionIndex)
{
}

void LookBackConnection::Send(NetMessage* message)
{
	message->m_sender = this;
	NetMessage* newMessage = new NetMessage(*message);
	m_messages.push_back(newMessage);
}

void LookBackConnection::Receive(NetMessage ** message)
{
	if (m_messages.empty())
		return;
	*message = m_messages.front();
	m_messages.pop_front();
}

std::vector<NetMessage *> LookBackConnection::Receive()
{
	std::vector<NetMessage *> result;
	while (!m_messages.empty())
	{
		NetMessage * message = m_messages.front();
		message->m_sender = this;
		result.push_back(message);
		m_messages.pop_front();
	}
	return result;
}

bool LookBackConnection::Connect()
{
	return true;
}