#include "TCPConnection.hpp"
#include "TCPSocket.hpp"


bool TCPConnection::IsValid()
{
	return m_socket->IsValid();
}

TCPConnection::TCPConnection(NetSession* owner, net_address_t address, uint8_t connectionIndex)
	: NetConnection(owner,address,connectionIndex)
	, m_socket(nullptr)
	, m_buffer()
{
}

void TCPConnection::Send(NetMessage* message)
{
	BinaryStream payload;
	payload.WriteBuffer((uint16_t)(message->GetSize() + 1));
	payload.WriteBuffer(message->m_messageTypeIndex);
	payload.MergeByteStream(*message);
	m_socket->Send(payload.GetBuffer(), payload.GetSize());
}

void TCPConnection::Receive(NetMessage ** message)
{
	byte_t bytes[NetMessage::MAX_PAYLOAD_SIZE];
	uint receivedByte = m_socket->Receive(bytes, NetMessage::MAX_PAYLOAD_SIZE);
	if (receivedByte > 0)
	{
		*message = new NetMessage(bytes, NetMessage::MAX_PAYLOAD_SIZE);
		(*message)->m_sender = this;
	}
}

std::vector<NetMessage *> TCPConnection::Receive()
{
	std::vector<NetMessage*> messages;
	byte_t bytes[NetMessage::MAX_PAYLOAD_SIZE];
	uint receivedByte = m_socket->Receive(bytes, NetMessage::MAX_PAYLOAD_SIZE);
	if (receivedByte == 0)
		return messages;
	uint usedByte = 0;
	m_buffer.m_stream.insert(m_buffer.m_stream.end(), bytes, bytes + receivedByte);
	if (!m_buffer.m_stream.empty())
	{
		uint16_t messageSize = m_buffer.ReadBufferNoProgress<uint16_t>() + 2;
		while (messageSize <= (uint16_t)m_buffer.m_stream.size())
		{
			NetMessage* message = new NetMessage(m_buffer.m_stream.data(), messageSize);
			message->m_sender = this;
			messages.push_back(message);
			m_buffer.m_stream.erase(m_buffer.m_stream.begin(), m_buffer.m_stream.begin() + messageSize);
			m_buffer.m_bufferOffset = 0;
			if(!m_buffer.m_stream.empty())
				messageSize = m_buffer.ReadBufferNoProgress<uint16_t>() + 2;
		}
	}
	/*if (receivedByte > 0)
	{
		while (usedByte < receivedByte)
		{
			NetMessage* message = new NetMessage(bytes + usedByte, NetMessage::MAX_PAYLOAD_SIZE);
			message->m_sender = this;
			messages.push_back(message);
			usedByte += message->m_payloadBytesUsed + 3;
		}
	}*/
	return messages;
}

bool TCPConnection::Connect()
{
	if (!m_socket)
		m_socket = new TCPSocket();
	bool result = m_socket->Join(m_address);
	if (result)
		m_socket->SetBlocking(false);
	return result;
}
